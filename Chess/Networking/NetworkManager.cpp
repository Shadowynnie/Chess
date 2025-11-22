#include "NetworkManager.h"
#include <iostream>

using std::string;
using std::cout;
using std::vector;

NetworkManager::NetworkManager() = default;

NetworkManager::~NetworkManager()
{
    Deinitialize();
}

vector<uint8_t> NetworkManager::CreatePacketData(MessageType type, const vector<uint8_t>& payload)
{
    vector<uint8_t> data;
    data.push_back(static_cast<uint8_t>(type));
    data.insert(data.end(), payload.begin(), payload.end());
    return data;
}

// ====================TESTING======================
void NetworkManager::sendTestPacket(ENetPeer* peer)
{
    string message = "Hello, ENet!";
    ENetPacket* packet = enet_packet_create(message.c_str(), message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
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
}

void NetworkManager::ServiceNetwork()
{
    ENetEvent event;
    ENetHost* host = _server ? _server : _client;
    
    while (true)
    {
        while (enet_host_service(host, &event, DELAY_MS) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                if (_server)
                {
                    cout << "A new client connected from "
                         << (event.peer->address.host & 0xFF) << "."
                         << ((event.peer->address.host >> 8) & 0xFF) << "."
                         << ((event.peer->address.host >> 16) & 0xFF) << "."
                         << ((event.peer->address.host >> 24) & 0xFF) << ":"
                        << event.peer->address.port << "\n";
                }
                else
                {
                    cout << "Successfully connected to server.\n";
                }
                _isConnected = true;
                _peer = event.peer;
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                // Handle received packet (event.packet)
                cout << "Received packet of length " << event.packet->dataLength << event.packet->data << "\n";
                const uint8_t* data = reinterpret_cast<const uint8_t*>(event.packet->data);
                size_t len = event.packet->dataLength;
                if (len >= 1)
                {
                    uint8_t msgType = data[0];
                    // Process message based on msgType
                    switch (static_cast<MessageType>(msgType))
                    {
                        case MessageType::MOVE:
                        cout << "Received MOVE packet message.\n";
                        break;
                        case MessageType::TURN_INFO:
                            cout << "Received TURN INFO packet message.\n";
                            break;
                        case MessageType::BOARD_STATE:
                            cout << "Received BOARD STATE packet message.\n";
                            break;
                        case MessageType::ACKNOWLEDGEMENT:
                            cout << "Received ACKNOWLEDGEMENT packet message.\n";
                            break;
                        case MessageType::TEST_PACKET:
                            cout << "Received TEST PACKET message.\n";
                            break;
                        case MessageType::PING:
                            cout << "Received PING message.\n";
                            cout << "Responding to PING...\n";
                            {
                                uint8_t pongMsg = static_cast<uint8_t>(MessageType::PING);
                                enet_peer_send(event.peer, 0, enet_packet_create(&pongMsg, 1, 0));
                            }
                            break;
                        default:
                            cout << "Received unknown packet message type: " << static_cast<int>(msgType) << "\n";
                            break;

                    }
                }
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                cout << (_server ? "A client disconnected.\n" : "Disconnected from server.\n");
                _isConnected = false;
                break;
            default:
                break;
            }
        }
        // After handling available events, ensure keep-alive: send ENet ping when idle
        if (_server && _isConnected)
        {
            if (_peer->state == ENET_PEER_STATE_CONNECTED)
            {
                uint8_t msg = static_cast<uint8_t>(MessageType::PING);
                enet_peer_send(_peer, 0, enet_packet_create(&msg, 1, 0));
            }
            cout << "Keeping connection alive...\n";
        }
    }

}

bool NetworkManager::IsConnected() const
{
    return _isConnected;
}