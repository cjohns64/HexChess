extends Control
@onready var game_over_pannel:Node = $Panel
@onready var game_over_text:Label = $Panel/Text

func _on_hex_chess_driver_game_over(state: HexChess.GameState, isWhiteTurn:bool) -> void:
	game_over_pannel.show();
	match state:
		HexChess.GameState.Checkmate:
			if isWhiteTurn:
				game_over_text.text = "Checkmate, Black Wins"
			else:
				game_over_text.text = "Checkmate, White Wins"
		HexChess.GameState.Stalemate:
			game_over_text.text = "Draw, Stalemate"
		HexChess.GameState.DeadPosistion:
			game_over_text.text = "Draw, Dead Posistion"
		HexChess.GameState.ThreefoldRepitition:
			game_over_text.text = "Draw, Threefold Repitition"
		HexChess.GameState.FiftyMoveRule:
			game_over_text.text = "Draw, 50 Move Rule"
		_:
			pass
