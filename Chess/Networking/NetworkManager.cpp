#include "NetworkManager.h"
#include <iostream>
#include <chrono>

using std::string;
using std::cout;
using std::cerr;
using std::vector;
using Clock = std::chrono::steady_clock;

NetworkManager::NetworkManager() = default;

NetworkManager::~NetworkManager()
{
    Deinitialize();
}

// Function to create packet data by serializing the message
vector<uint8_t> NetworkManager::CreatePacketData( const MoveMessage& mvMsg)
{
    vector<uint8_t> data;
    data.reserve(1 + sizeof(mvMsg)); // Reserve space for type + payload
    data.push_back(static_cast<uint8_t>(MessageType::MOVE));
    // Serialization logic for different message types would go here
    const uint8_t* rawBytesPtr = reinterpret_cast<const uint8_t*>(&mvMsg);
    data.insert(data.end(), rawBytesPtr, rawBytesPtr + sizeof(mvMsg));
    return data;
}

// Function to parse packet data by deserializing the message
std::optional<MoveMessage> NetworkManager::ParsePacketData(const uint8_t* data, size_t length)
{
    if (length < 1 + sizeof(MoveMessage))
        return std::nullopt; // Not enough data

    MoveMessage mvMsg;
    std::memcpy(&mvMsg, data + 1, sizeof(MoveMessage));
    return mvMsg;
}

// ====================TESTING======================
void NetworkManager::SendTestPacket(ENetPeer* peer)
{
    string message = "Hello, ENet!";
    ENetPacket* packet = enet_packet_create(message.c_str(), message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void NetworkManager::SendMovePacket(ENetPeer* peer, const MoveMessage& mvMsg)
{
    vector<uint8_t> packetData = CreatePacketData(mvMsg);
    ENetPacket* packet = enet_packet_create(packetData.data(), packetData.size(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}
// =================================================

bool NetworkManager::Initialize()
{
    if (enet_initialize() != 0)
    {
        return false;
    }
    return true;
}

void NetworkManager::Deinitialize()
{
    Disconnect();
    enet_deinitialize();
}

bool NetworkManager::HostGame(unsigned short port)
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;
    _server = enet_host_create(&address, MAX_CLIENTS, CHANNELS, 0, 0);
    if (_server == nullptr)
    {
        cout << "Failed to create ENet server host.\n";
        return false;
    }
    return true;
}

bool NetworkManager::ConnectToGame(const string& address, unsigned short port)
{
    _client = enet_host_create(nullptr, 1, CHANNELS, 0, 0);
    if (_client == nullptr)
    {
        cout << "Failed to create ENet client host.\n";
        return false;
    }

    ENetAddress serverAddress;
    enet_address_set_host(&serverAddress, address.c_str());
    serverAddress.port = port;
    _peer = enet_host_connect(_client, &serverAddress, CHANNELS, 0);

    if (_peer == nullptr)
    {
        cout << "Failed to initiate connection to server.\n";
        return false;
    }
    return true;
}

void NetworkManager::Disconnect()
{
    if (_peer != nullptr)
    {
        enet_peer_disconnect(_peer, 0);
        _peer = nullptr;
    }
    if (_server != nullptr)
    {
        enet_host_destroy(_server);
        _server = nullptr;
    }
    if (_client != nullptr)
    {
        enet_host_destroy(_client);
        _client = nullptr;
    }
    _isConnected = false;
    _isServiceRunning = false;
}

void NetworkManager::ServiceNetwork()
{
    ENetEvent event;
    ENetHost* host = _server ? _server : _client;
    _isServiceRunning = true;

    while (_isServiceRunning && host)
    {
        // Poll frequently for events (short timeout)
        while (enet_host_service(host, &event, DELAY_MS) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                // store peer and mark connected
                // For server, multiple peers may connect; store last for convenience
                _peer = event.peer;
                _isConnected = true;
                _lastPing = Clock::now();
                cout << "Peer connected: " << event.peer->address.host << ":" << event.peer->address.port << "\n";
                break;

            case ENET_EVENT_TYPE_RECEIVE:
            {
                const uint8_t* data = reinterpret_cast<const uint8_t*>(event.packet->data);
                size_t length = event.packet->dataLength;
                if (length >= 1)
                {
                    uint8_t msgType = data[0];

                    if (msgType == static_cast<uint8_t>(MessageType::MOVE))
                    {
                        auto moveData = ParsePacketData(data, length);
                        if (!moveData)
                        {
                            cerr << "Invalid MOVE packet received\n";
                        }
                        else
                        {
                            /*
                            MoveMessage mv = *mvOpt;
                            if (m_isServer)
                            {
                                // Server: validate move (no state changes here)
                                bool valid = GameManager::ServerValidateAndApplyMove(mv);
                                if (valid)
                                {
                                    // Enqueue for the local main/UI thread so server applies move and updates sprites there
                                    PushIncomingMove(mv);

                                    // Broadcast the validated move to all connected peers
                                    auto buffer = SerializeMove(mv);
                                    ENetPacket* pkt = enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE);
                                    if (pkt)
                                    {
                                        enet_host_broadcast(m_host, 0, pkt);
                                        enet_host_flush(m_host);
                                    }
                                }
                                else
                                {
                                    cerr << "Server rejected incoming move request\n";
                                }
                            }
                            else
                            {
                                // Client: server (or host) forwarded a validated move -> queue for main thread
                                PushIncomingMove(mv);
                            }*/
                            cout << "Received MOVE packet: from (" << static_cast<int>(moveData->fromX) << "," << static_cast<int>(moveData->fromY)
                                << ") to (" << static_cast<int>(moveData->toX) << "," << static_cast<int>(moveData->toY) << ")\n";
                        }
                    }
                    else if (msgType == static_cast<uint8_t>(MessageType::BOARD_STATE))
                    {
                        string fen(reinterpret_cast<const char*>(data + 1), length - 1);
                        cout << "Received BOARD STATE string: " << fen << "\n";
                    }
                    else if (msgType == static_cast<uint8_t>(MessageType::PING))
                    {
                        cout << "Received PING message.\n";
                        // optional: update last-ping timestamp or reply with ACK
                        _lastPing = Clock::now();
                    }
                }
                enet_packet_destroy(event.packet);
            }
            break;

            case ENET_EVENT_TYPE_DISCONNECT:
                cout << "Peer disconnected.\n";
                // if the peer that disconnected is our stored peer, clear it
                if (_peer == event.peer)
                {
                    _peer = nullptr;
                    _isConnected = false;
                }
                event.peer->data = nullptr;
                break;
            default:
                break;
            }
        }

        // After handling available events, ensure keep-alive: send ENet ping when idle
        if (_isServiceRunning && _isConnected && _server)
        {
            auto now = Clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastPing).count();
            if (elapsed >= _pingIntervalMs)
            {
                // Use ENet's ping helper to update RTT and keep connection alive (ping all peers)
                if (host)
                {
                    // iterate peers and ping
                    if (_peer->state == ENET_PEER_STATE_CONNECTED)
                    {
                        uint8_t msg = static_cast<uint8_t>(MessageType::PING);
                        enet_peer_send(_peer, 0, enet_packet_create(&msg, 1, 0));
                    }
                    enet_host_flush(host);
                }
                _lastPing = now;
                cout << "Sent keep-alive ping.\n";
            }
        }
    }
}

bool NetworkManager::IsConnected() const
{
    return _isConnected;
}