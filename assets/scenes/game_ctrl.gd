extends Node3D
var game_scene
var game_asset = preload("res://assets/scenes/world.tscn")

func _ready() -> void:
	NewGame()

func _on_world_restart_pressed() -> void:
	NewGame()
	
func NewGame() -> void:
	if game_scene: game_scene.queue_free()
	game_scene = game_asset.instantiate()
	game_scene.restart_pressed.connect(_on_world_restart_pressed)
	add_child(game_scene)
