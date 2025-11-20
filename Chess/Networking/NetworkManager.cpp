// Chess/Networking/NetworkManager.cpp
#include "NetworkManager.h"
#include <cstring>
#include <iostream>

using std::cout;
using std::cerr;
using std::vector;
using std::memcpy;
using Clock = std::chrono::steady_clock;

NetworkManager::~NetworkManager()
{
    if (m_host)
        enet_host_destroy(m_host);
}

bool NetworkManager::InitializeHostAsServer(uint16_t port)
{
    if (enet_initialize() != 0)
    {
        cerr << "ENet init failed\n";
        return false;
    }
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;
    m_host = enet_host_create(&address, 2, 2, 0, 0);
    if (!m_host)
    {
        cerr << "Failed to create ENet server host\n";
        return false;
    }
    m_running = true;
    m_connected = false;
    return true;
}

bool NetworkManager::InitializeHostAsClient(const char* host, uint16_t port)
{
    if (enet_initialize() != 0)
    {
        cerr << "ENet init failed\n";
        return false;
    }
    m_host = enet_host_create(nullptr /* client host */, 1, 2, 0, 0);
    if (!m_host)
    {
        cerr << "Failed to create ENet client host\n";
        return false;
    }
    ENetAddress address;
    enet_address_set_host(&address, host);
    address.port = port;
    // request connection; the connect result is delivered in ServiceLoop via CONNECT event
    ENetPeer* peer = enet_host_connect(m_host, &address, 2, 0);
    if (!peer)
    {
        cerr << "No available peers for initiating connection\n";
        return false;
    }
    // peer remains managed by ENet; ServiceLoop will set m_peer when ENET_EVENT_TYPE_CONNECT occurs
    m_running = true;
    m_connected = false;
    return true;
}

vector<uint8_t> NetworkManager::SerializeMove(const MoveMessage& moveMsg)
{
    // message layout: [type (1)] + payload
    vector<uint8_t> buffer;
    buffer.reserve(1 + sizeof(MoveMessage));
    buffer.push_back(static_cast<uint8_t>(MessageType::MOVE));
    const uint8_t* rawBytesPtr = reinterpret_cast<const uint8_t*>(&moveMsg);
    buffer.insert(buffer.end(), rawBytesPtr, rawBytesPtr + sizeof(MoveMessage));
    return buffer;
}

std::optional<MoveMessage> NetworkManager::DeserializeMove(const uint8_t* data, size_t len)
{
    if (len < 1 + sizeof(MoveMessage))
        return std::nullopt;
    uint8_t msgType = data[0];
    if (msgType != static_cast<uint8_t>(MessageType::MOVE))
        return std::nullopt;
    MoveMessage mvMsg;
    memcpy(&mvMsg, data + 1, sizeof(MoveMessage));
    // Basic sanity checks
    if (mvMsg.fromX > 7 || mvMsg.fromY > 7 || mvMsg.toX > 7 || mvMsg.toY > 7)
        return std::nullopt;
    return mvMsg;
}

bool NetworkManager::SendMoveReliable(ENetPeer* peer, const MoveMessage& moveMsg)
{
    if (!peer)
        return false;
    auto buffer = SerializeMove(moveMsg);
    ENetPacket* pkt = enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE);
    if (!pkt)
        return false;
    if (enet_peer_send(peer, 0 /*channel 0 reliable*/, pkt) != 0)
    {
        enet_packet_destroy(pkt);
        return false;
    }
    enet_host_flush(peer->host);
    return true;
}

bool NetworkManager::SendTextReliable(ENetPeer* peer, const string& text, MessageType type)
{
    if (!peer)
        return false;
    if (type == MessageType::FULL_FEN)
    {
        // prefix the MessageType byte so receiver knows the kind
        vector<uint8_t> buffer;
        buffer.reserve(1 + text.size());
        buffer.push_back(static_cast<uint8_t>(type)); // message type byte first
        buffer.insert(buffer.end(), text.begin(), text.end()); // then text bytes
        ENetPacket* pkt = enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE);
        if (!pkt)
            return false;
        if (enet_peer_send(peer, 0, pkt) != 0)
        {
            enet_packet_destroy(pkt);
            return false;
        }
        enet_host_flush(peer->host);
        return true;
    }
    return false;
}

void NetworkManager::ServiceLoop()
{
    ENetEvent event;
    while (m_running && m_host)
    {
        // Poll frequently for events (short timeout)
        while (enet_host_service(m_host, &event, POLL_INTERVAL_MS) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                    // store peer and mark connected
                    m_peer = event.peer;
                    m_connected = true;
                    m_lastPing = Clock::now();
                    cout << "Peer connected.\n";
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                {
                    const uint8_t* data = reinterpret_cast<const uint8_t*>(event.packet->data);
                    size_t len = event.packet->dataLength;
                    if (len >= 1)
                    {
                        uint8_t msgType = data[0];
                        if (msgType == static_cast<uint8_t>(MessageType::MOVE))
                        {
                            auto mvOpt = DeserializeMove(data, len);
                            if (mvOpt)
                            {
                                MoveMessage mv = *mvOpt;
                                cout << "Received move: (" << int(mv.fromX) << "," << int(mv.fromY)
                                    << ") -> (" << int(mv.toX) << "," << int(mv.toY) << ")\n";
                            }
                        }
                        else if (msgType == static_cast<uint8_t>(MessageType::FULL_FEN))
                        {
                            string fen(reinterpret_cast<const char*>(data + 1), len - 1);
                            cout << "Received FEN: " << fen << "\n";
                        }
                        else if (msgType == static_cast<uint8_t>(MessageType::PING))
                        {
                            // optional: update last-ping timestamp or reply with ACK
                            m_lastPing = Clock::now();
                        }
                    }
                    enet_packet_destroy(event.packet);
                }
                break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    cout << "Peer disconnected.\n";
                    // if the peer that disconnected is our stored peer, clear it
                    if (m_peer == event.peer)
                    {
                        m_peer = nullptr;
                        m_connected = false;
                    }
                    event.peer->data = nullptr;
                    break;
                default:
                    break;
            }
        }

        // After handling available events, ensure keep-alive: send ENet ping when idle
        if (m_running && m_connected && m_peer)
        {
            auto now = Clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastPing).count();
            if (elapsed >= m_pingIntervalMs)
            {
                // Use ENet's ping helper to update RTT and keep connection alive
                enet_peer_ping(m_peer);
                enet_host_flush(m_host);
                m_lastPing = now;
                cout << "Sent keep-alive ping.\n";
            }
        }
    }
}
void NetworkManager::StopServiceLoop()
{
    m_running = false;
    // let service loop exit; destruction will enet_host_destroy
}