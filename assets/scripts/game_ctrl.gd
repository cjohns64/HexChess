extends Node3D
var game_scene:Node3D
#var other_scene:Window
var game_asset := preload("res://assets/scenes/world_one_window.tscn")
#const SECOND_WINDOW = preload("res://assets/scenes/second_window.tscn")
var is_server:bool = false
var game_ready:bool = false
var is_white:bool = false
var send_color_info:bool = false

@onready var main_menu: Control = $MainMenu
@onready var network_manager: Node = $hexchess_network_manager

func _on_world_restart_pressed() -> void:
	if game_scene.hex_chess_driver.multiplayer.multiplayer_peer:
		# close connection
		game_scene.hex_chess_driver.multiplayer.multiplayer_peer = OfflineMultiplayerPeer.new()
	if game_scene: game_scene.queue_free()
	main_menu._show()
	main_menu.init_menu()
	
func NewGame() -> void:
	if game_scene: game_scene.queue_free()
	main_menu._hide()
	game_scene = game_asset.instantiate()
	game_scene.restart_pressed.connect(_on_world_restart_pressed)
	add_child(game_scene)

func _on_main_menu_start_local(num_windows: int) -> void:
	if num_windows == 1:
		NewGame()
		game_scene.hex_chess_driver.IS_LOCAL = true
	else:
		pass
		#game_scene = game_asset.instantiate()
		#var player2_scene:Window = SECOND_WINDOW.instantiate()
		##get_viewport().set_embedding_subwindows(false)
		#get_window().title = "HexChess - White"
		#player2_scene.title = "HexChess - Black"
		#main_menu.hide()
		#add_child(game_scene)
		#add_child(player2_scene)
		## white player
		#var peer1:ENetMultiplayerPeer = network_manager.get_multiplayer_api_two_window(true)
		#game_scene.hex_chess_driver.multiplayer.multiplayer_peer = peer1
		#game_scene.hex_chess_driver.IsWhitePlayer = true
		## black player
		#var peer2:ENetMultiplayerPeer = network_manager.get_multiplayer_api_two_window(false)
		#player2_scene.hex_chess_driver.multiplayer.multiplayer_peer = peer2
		#player2_scene.hex_chess_driver.IsWhitePlayer = false

func _on_main_menu_start_online(server: bool, is_white_player: bool, ip_addr:String="127.0.0.1") -> void:
	self.is_server = server
	self.is_white = is_white_player
	set_network_notification("waiting for connection..")
	#NewGame()
	if server:
		self.multiplayer.multiplayer_peer = network_manager.start_server()
		#game_scene.hex_chess_driver.IsWhitePlayer = is_white_player
	else:
		self.multiplayer.multiplayer_peer = network_manager.start_client(ip_addr)

@rpc("authority")
func require_color(is_white_player:bool) -> void:
	#self.game_scene.hex_chess_driver.IsWhitePlayer = is_white_player
	self.is_white = is_white_player
	self.game_ready = true
	print("set player to %s" % ("White" if is_white_player else "Black"))

func _on_hexchess_network_manager_peer_connection_change(_ID: int, con: bool) -> void:
	if con:
		# new connection
		if self.is_server:
			self.send_color_info = true # send color sync order after a delay
			self.game_ready = true
			print("server player is %s" % ("White" if self.is_white else "Black"))
		else:
			print("client player is %s" % ("White" if self.is_white else "Black"))
	else:
		# lost connection
		print("client player is %s" % ("White" if self.is_white else "Black"))
		set_network_notification("lost connection with other player, waiting for reconnect...")

func _process(_delta: float) -> void:
	if self.game_ready:
		# launch game
		NewGame()
		hide_network_notification()
		print("Starting Game")
		self.game_scene.hex_chess_driver.game_start = true
		self.game_scene.hex_chess_driver.IsWhitePlayer = self.is_white
		self.game_scene.set_ui_turn_labels()
		game_ready = false
	elif self.send_color_info and self.is_server:
		self.send_color_info = false
		require_color.rpc(not self.is_white)

func set_network_notification(txt:String, end_button:bool=false) -> void:
	# disable interaction
	if self.game_scene and self.game_scene.hex_chess_driver:
		self.game_scene.hex_chess_driver.menu_active += 1
	main_menu.network_panel.show()
	if end_button: main_menu.network_pannel_button.show()
	main_menu.network_pannel_text.text = txt

func hide_network_notification() -> void:
	if self.game_scene and self.game_scene.hex_chess_driver:
		self.game_scene.hex_chess_driver.menu_active -= 1
	main_menu.network_pannel_button.hide()
	main_menu.network_panel.hide()

func _on_main_menu_end_game() -> void:
	self._on_world_restart_pressed()
