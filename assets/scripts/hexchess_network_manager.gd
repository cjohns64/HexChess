extends Node

@export var LOCAL_PORT = 1909
var server_ip:String
signal PeerConnectionChange(ID:int, con:bool)
signal DeSyncDraw
var disconnect_timer:float = 0.0
var is_peer_disconnected:bool = false
@export var max_disconnect_timer:float = 10.0

func start_server() -> ENetMultiplayerPeer:
	# Create server.
	var server:ENetMultiplayerPeer = ENetMultiplayerPeer.new()
	#server.set_bind_ip(LOCAL_IP)
	server.create_server(LOCAL_PORT, 1)
	server.peer_connected.connect(_on_peer_connect)
	server.peer_disconnected.connect(_on_peer_disconnect)
	server_ip = IP.get_local_addresses()[0]
	print("Server IP is: ", server_ip)
	return server
	
func start_client(IP_Address:String) -> ENetMultiplayerPeer:
	# Create client.
	var client:ENetMultiplayerPeer = ENetMultiplayerPeer.new()
	client.create_client(IP_Address, LOCAL_PORT)
	client.peer_connected.connect(_on_peer_connect)
	client.peer_disconnected.connect(_on_peer_disconnect)
	return client

func _on_peer_connect(ID:int) -> void:
	print("peer connect %d" % [ID])
	is_peer_disconnected = false
	disconnect_timer = 0.0
	PeerConnectionChange.emit(ID, true)
	
func _on_peer_disconnect(ID:int) -> void:
	print("peer disconnect %d" % [ID])
	is_peer_disconnected = true
	PeerConnectionChange.emit(ID, false)
	
func _process(delta: float) -> void:
	if is_peer_disconnected:
		disconnect_timer += delta
	if is_peer_disconnected and disconnect_timer > max_disconnect_timer:
		is_peer_disconnected = false
		DeSyncDraw.emit()
