extends Node3D
var game_scene:Node3D
var game_asset := preload("res://assets/scenes/world_one_window.tscn")
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
	
func NewGame(set_turn_labels:bool=true) -> void:
	if game_scene: game_scene.queue_free()
	main_menu._hide()
	game_scene = game_asset.instantiate()
	game_scene.restart_pressed.connect(_on_world_restart_pressed)
	add_child(game_scene)
	hide_network_notification()
	print("Starting Game")
	self.game_scene.hex_chess_driver.game_start = true
	self.game_scene.hex_chess_driver.IsWhitePlayer = self.is_white
	if set_turn_labels: self.game_scene.set_ui_turn_labels()

func _on_main_menu_start_local(is_white_player: bool) -> void:
	self.is_white = is_white_player
	NewGame(false)
	game_scene.hex_chess_driver.IS_LOCAL = true
	
func _on_main_menu_start_online(server: bool, is_white_player: bool, ip_addr:String="127.0.0.1") -> void:
	self.is_server = server
	self.is_white = is_white_player
	if server:
		self.multiplayer.multiplayer_peer = network_manager.start_server()
		set_network_notification("Your IP is %s\nwaiting for connection.." % (network_manager.server_ip))
		#game_scene.hex_chess_driver.IsWhitePlayer = is_white_player
	else:
		set_network_notification("waiting for connection..")
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
		if not self.game_scene.hex_chess_driver.game_over:
			print("client player is %s" % ("White" if self.is_white else "Black"))
			set_network_notification("lost connection with other player, waiting for reconnect...")

func _process(_delta: float) -> void:
	if self.game_ready:
		# launch game
		NewGame()
		game_ready = false
	elif self.send_color_info and self.is_server:
		self.send_color_info = false
		require_color.rpc(not self.is_white)

func set_network_notification(txt:String, end_button:bool=false) -> void:
	# disable interaction
	if self.game_scene and self.game_scene.hex_chess_driver:
		print("net menu active")
		self.game_scene.hex_chess_driver.menu_active += 1
	main_menu.network_panel.show()
	if end_button: main_menu.network_pannel_button.show()
	main_menu.network_pannel_text.text = txt

func hide_network_notification() -> void:
	if self.game_scene and self.game_scene.hex_chess_driver:
		print("net menu inactive")
		if self.game_scene.hex_chess_driver.menu_active > 0:
			self.game_scene.hex_chess_driver.menu_active -= 1
	main_menu.network_pannel_button.hide()
	main_menu.network_panel.hide()

func _on_main_menu_end_game() -> void:
	self._on_world_restart_pressed()
