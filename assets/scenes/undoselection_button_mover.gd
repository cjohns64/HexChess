extends Panel

func _on_hex_chess_driver_move_selection_button(new_location: Vector2) -> void:
	#print("setting new location to: ", new_location)
	global_position = new_location
	visible = true

func _on_hex_chess_driver_disable_undo_button() -> void:
	visible = false
