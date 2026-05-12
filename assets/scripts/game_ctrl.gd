extends Node3D
var game_scene:Node3D
#var other_scene:Window
var game_asset := preload("res://assets/scenes/world_one_window.tscn")
#const SECOND_WINDOW = preload("res://assets/scenes/second_window.tscn")

@onready var main_menu: Control = $MainMenu
@onready var network_manager: Node = $hexchess_network_manager

func _on_world_restart_pressed() -> void:
	if game_scene.hex_chess_driver.multiplayer.multiplayer_peer:
		# close connection
		game_scene.hex_chess_driver.multiplayer.multiplayer_peer = OfflineMultiplayerPeer.new()
	if game_scene: game_scene.queue_free()
	main_menu.show()
	main_menu.init_menu()
	
func NewGame() -> void:
	if game_scene: game_scene.queue_free()
	main_menu.hide()
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

func _on_main_menu_start_online(server: bool, is_white_player: bool) -> void:
	NewGame()
	if server:
		self.multiplayer.multiplayer_peer = network_manager.start_server()
		game_scene.hex_chess_driver.IsWhitePlayer = is_white_player
	else:
		self.multiplayer.multiplayer_peer = network_manager.start_client()
		# TODO force opposite of server
		game_scene.hex_chess_driver.IsWhitePlayer = is_white_player
