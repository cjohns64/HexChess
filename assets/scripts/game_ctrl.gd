extends Node3D
var game_scene
var game_asset
const WORLD_ONE_WINDOW = preload("res://assets/scenes/world_one_window.tscn")
@onready var main_menu: Control = $MainMenu

func _on_world_restart_pressed() -> void:
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
		game_asset = WORLD_ONE_WINDOW
		NewGame()
		game_scene.hex_chess_driver.IS_LOCAL = true
	else:
		pass
