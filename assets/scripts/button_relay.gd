extends Node3D

@onready var ui: Control = $UI
signal restart_pressed()

func _on_button_pressed() -> void:
	print("Restart Button Pressed")
	restart_pressed.emit()

func activate_validation_pannel() -> Label:
	return ui.activate_validation_pannel()

func _on_hex_chess_driver_move_validation(state: bool) -> void:
	ui.on_validation_move(state)
