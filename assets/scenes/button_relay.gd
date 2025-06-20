extends Node3D

signal restart_pressed()

func _on_button_pressed() -> void:
	print("Restart Button Pressed")
	restart_pressed.emit()
