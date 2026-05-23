extends Node3D

@onready var ui: Control = $UI
signal restart_pressed()
@onready var hex_chess_driver: HexChess = $Chessboard/HexChessDriver
@export var this_player_turn_text:String = "Your Turn"
@export var other_player_turn_text:String = "Opponent's Turn"

func _on_button_pressed() -> void:
	print("Restart Button Pressed")
	restart_pressed.emit()

func activate_validation_pannel() -> Label:
	return ui.activate_validation_pannel()

func _on_hex_chess_driver_move_validation(state: bool) -> void:
	ui.on_validation_move(state)

func set_ui_turn_labels() -> void:
	if hex_chess_driver.IsWhitePlayer:
		ui.wturn_text.text = this_player_turn_text
		ui.bturn_text.text = other_player_turn_text
	else:
		ui.bturn_text.text = this_player_turn_text
		ui.wturn_text.text = other_player_turn_text
