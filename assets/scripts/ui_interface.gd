extends Control
@onready var game_over_pannel:Node = $Panel
@onready var game_over_text:Label = $Panel/Text
@onready var promotion_pannel:Node = $PromotionPanel
@onready var wturn_text:Label = $TurnPanel/WhiteTurn
@onready var bturn_text:Label = $TurnPanel/BlackTurn

signal promotion_selected(selection:int)

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

func activate_promotion_pannel() -> void:
	promotion_pannel.show()

func _on_queen_button_pressed() -> void:
	promotion_selected.emit(1)
	promotion_pannel.hide()

func _on_rook_button_pressed() -> void:
	promotion_selected.emit(2)
	promotion_pannel.hide()

func _on_knight_button_pressed() -> void:
	promotion_selected.emit(4)
	promotion_pannel.hide()

func _on_bishop_button_pressed() -> void:
	promotion_selected.emit(3)
	promotion_pannel.hide()
	
func on_turn_changed(is_white_turn:bool) -> void:
	if is_white_turn:
		wturn_text.show()
		bturn_text.hide()
	else:
		wturn_text.hide()
		bturn_text.show()
