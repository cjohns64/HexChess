extends Node3D

@onready var ui: Control = $UI
signal restart_pressed()
@onready var player1_driver: HexChess = $Chessboard/HexChessDriver
@onready var player2_driver: HexChess = $"Chessboard-other/HexChessDriver"

func _ready() -> void:
	player1_driver.IsWhitePlayer = false
	player2_driver.IsWhitePlayer = true
	
func message_other_player(message:String, this_player:bool) -> bool:
	if this_player:
		return player1_driver.ProcessMoveRequest(message)
	else:
		return player2_driver.ProcessMoveRequest(message)

func _on_button_pressed() -> void:
	print("Restart Button Pressed")
	restart_pressed.emit()

func activate_validation_pannel() -> Label:
	return ui.activate_validation_pannel()

func _on_hex_chess_driver_move_validation(state: bool) -> void:
	ui.on_validation_move(state)
