extends Window
const WORLD_ONE_WINDOW = preload("res://assets/scenes/world_one_window.tscn")
var hex_chess_driver: HexChess

func _ready() -> void:
	#transient = false # child of main window
	close_requested.connect(queue_free) # close when window closes
	var game_scene:Node3D = WORLD_ONE_WINDOW.instantiate()
	add_child(game_scene)
	hex_chess_driver = game_scene.hex_chess_driver
	
