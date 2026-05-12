extends Node

@export var LOCAL_IP = "127.0.0.1"
@export var LOCAL_PORT = 1909

func start_server() -> ENetMultiplayerPeer:
	# Create server.
	var server:ENetMultiplayerPeer = ENetMultiplayerPeer.new()
	#server.set_bind_ip(LOCAL_IP)
	server.create_server(LOCAL_PORT, 1)
	server.peer_connected.connect(_on_peer_connect)
	server.peer_disconnected.connect(_on_peer_disconnect)
	return server
	
func start_client() -> ENetMultiplayerPeer:
	# Create client.
	var client:ENetMultiplayerPeer = ENetMultiplayerPeer.new()
	client.create_client(LOCAL_IP, LOCAL_PORT)
	client.peer_connected.connect(_on_peer_connect)
	client.peer_disconnected.connect(_on_peer_disconnect)
	return client

func _on_peer_connect(ID:int) -> void:
	print("peer connect %d" % [ID])
	
func _on_peer_disconnect(ID:int) -> void:
	print("peer disconnect %d" % [ID])
