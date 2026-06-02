extends GDHexChessDriver
class_name HexChess

var DEMO_MODE:bool = false
var DEMO_TIMER:float = 1.0
signal gameOver(state:GameState, isWhiteTurn:bool)
signal activate_promotion()
signal play_clank_sound(type:int)
signal turn_changed(is_white_turn:bool)
signal move_validation(state:bool)
var IS_LOCAL:bool = false
var FAILURE_COUNT_MAX:int = 3
var current_failure_count:int = 0

enum ActionType {NoAction, Selectable, Move, MoveAndSelect}
enum PieceType {King, Queen, Rook, Bishop, Knight, Pawn, NoPiece}
enum GameState {Running, Checkmate, Stalemate, DeadPosistion, ThreefoldRepitition, FiftyMoveRule}
enum RPC_STATE {NoAction, MoveV_PROCESS, MoveV_RESULT, Promotion}
var RPC_avalibile:bool = true
var call_state_ctrl:RPC_STATE = RPC_STATE.NoAction
var WhitePieces:Dictionary[PieceType, PackedScene] = {
	PieceType.King:preload("res://assets/scenes/pieces/King-White.tscn"),
	PieceType.Queen:preload("res://assets/scenes/pieces/Queen-White.tscn"),
	PieceType.Rook:preload("res://assets/scenes/pieces/Rook-White.tscn"),
	PieceType.Bishop:preload("res://assets/scenes/pieces/Bishop-White.tscn"),
	PieceType.Knight:preload("res://assets/scenes/pieces/Knight-White.tscn"),
	PieceType.Pawn:preload("res://assets/scenes/pieces/Pawn-White.tscn")
	}
var BlackPieces:Dictionary[PieceType, PackedScene] = {
	PieceType.King:preload("res://assets/scenes/pieces/King-Black.tscn"),
	PieceType.Queen:preload("res://assets/scenes/pieces/Queen-Black.tscn"),
	PieceType.Rook:preload("res://assets/scenes/pieces/Rook-Black.tscn"),
	PieceType.Bishop:preload("res://assets/scenes/pieces/Bishop-Black.tscn"),
	PieceType.Knight:preload("res://assets/scenes/pieces/Knight-Black.tscn"),
	PieceType.Pawn:preload("res://assets/scenes/pieces/Pawn-Black.tscn")
	}
signal move_selection_button(new_location:Vector2)
signal disable_undo_button()
@export var undo_button_offset:Vector3
@export var validation_relay: Node3D
var IsWhitePlayer: bool = true
var CurrentSelection: String = ""
var validation_label:Label
var move_data:String

class Chessboard:
	var _internal_array: Array[TileObject] = []
	func _init(root:Node3D, driver:HexChess):
		# instantiate an empty chessboard
		var x:int = 6
		var s:int = 0
		var n:int = 1 # total count
		for i in 11:
			for j in x:
				# add element to array
				#print("adding Tile%d" % n)
				var tile_node:Node3D = root.get_node("Tile%d" % n)
				var tile_script:TileInteraction = tile_node as TileInteraction
				# set the material
				tile_script.SetMaterial((i + j + s) % 3)
				# set the rank and file
				tile_script.rank = i
				tile_script.file = j + s
				# connect to on clicked signal
				tile_script.tile_clicked.connect(func(rank:int, file:int): driver.OnTileClicked(rank, file))
				#tile_script.PrintStats()
				# add to reference
				_internal_array.push_back(
					TileObject.new(
						i, 
						j+s,
						tile_node
						)
					)
				n += 1
			# files grow by one on the end before rank 5,
			# and shrink by one at the start after rank 5.
			if i >= 5:
				x -= 1
				s += 1
			else:
				x += 1
		
	func get_tile(rank:int, file:int) -> TileObject:
		# the internal array is 1D but the hexagon board is non-uniform 2D
		match rank:
			0:
				#file in range 0-5
				return _internal_array[file]
			1:
				#file in range 0-6
				return _internal_array[file + 6]
			2:
				#file in range 0-7
				return _internal_array[file + 13]
			3:
				#file in range 0-8
				return _internal_array[file + 21]
			4:
				#file in range 0-9
				return _internal_array[file + 30]
			5:
				#file in range 0-10
				return _internal_array[file + 40]
			6:
				#file in range 1-10
				return _internal_array[file + 50]
			7:
				#file in range 2-10
				return _internal_array[file + 59]
			8:
				#file in range 3-10
				return _internal_array[file + 67]
			9:
				#file in range 4-10
				return _internal_array[file + 74]
			10:
				#file in range 5-10
				return _internal_array[file + 80]
			_:
				print("ERROR :: Default returned from tile lookup!")
				return _internal_array[0] # default should never happen

var hexboard:Chessboard
var board_mesh: Node3D

class ChessPiece:
	var player:bool
	var type:PieceType
	func _init(_is_white_player:bool, _type:PieceType) -> void:
		player = _is_white_player
		type = _type

class TileObject:
	var rank:int
	var file:int
	var tile_instance:Node3D
	var piece:ChessPiece
	var instance:Node3D
	func _init(_rank:int, _file:int, _tile_instance:Node3D):
		rank = _rank
		file = _file
		#print("init tile object: %s %d %d" % [_tile_instance.name, _rank, _file])
		tile_instance = _tile_instance

func _ready() -> void:
	board_mesh = $"../BoardMesh"
	self.__init_hexboard.call_deferred(board_mesh) # board_mesh is initialized after hexchess driver
	
func __init_hexboard(_board_mesh:Node3D) -> void:
	hexboard = Chessboard.new(board_mesh, self)

var round_in_process:bool = false
var game_over:bool = false
var menu_active:int = 0
var game_start:bool = false
var round_num:int = -1 # _ready will run the first round
func _process(_delta: float) -> void:
	if not game_start: return
	if !game_over and !round_in_process:
		current_failure_count = 0
		round_in_process = true
		# start next round
		RoundSetup()
		round_num += 1
		turn_changed.emit(round_num % 2 == 0)

		#print("starting round ", round_num)
		if ReturnGameState() == GameState.Running:
			disable_undo_button.emit()
			#print("finding selections")
			GetSelectableTiles()
			#print("updating board")
			UpdateBoard()
		else:
			# game over
			game_over = true
			menu_active += 1
			ClearHighlights()
			#print("GAME OVER")
			gameOver.emit(ReturnGameState(), round_num % 2 == 0)
		if not isActivePlayerTurn():
			call_state_ctrl = RPC_STATE.MoveV_PROCESS # ready for opponent's move request
			RPC_avalibile = true

func under_retry_count() -> bool:
	current_failure_count += 1
	if current_failure_count >= FAILURE_COUNT_MAX:
		return false
	return true

func ForceDraw() -> void:
	# game over
	game_over = true
	menu_active += 1
	ClearHighlights()
	#print("GAME OVER")
	gameOver.emit(GameState.Stalemate, round_num % 2 == 0)
	
@rpc("any_peer")
func ProcessMoveRequest(responce:String) -> void:
	if not IS_LOCAL and not RPC_avalibile and call_state_ctrl != RPC_STATE.MoveV_PROCESS: return
	RPC_avalibile = false
	if DEMO_MODE: await get_tree().create_timer(DEMO_TIMER).timeout
	# input validation
	if len(responce) != 4:
		print("response has len", len(responce))
		return
	# decode move
	var other_move:Array[int] = [responce[0].hex_to_int(), responce[1].hex_to_int(), responce[2].hex_to_int(), responce[3].hex_to_int()]
	# validate range
	var valid_range:bool = true
	for x in range(4):
		if other_move[x] > 10 or other_move[x] < 0:
			valid_range = false
			print("invalid range, ", other_move[x])
			break
	# check selection
	var action:ActionType = ParseActionType(GetActionOnTile(other_move[0], other_move[1]))
	if IS_LOCAL:
		MoveValidationResult(true)
	else:
		if not valid_range or action != ActionType.Selectable:
			MoveValidationResult.rpc(false) # invalid selection
			if not under_retry_count():
				ForceDraw()
			return
		# select tile
		GetMoveTiles(other_move[0], other_move[1]) # notify driver of piece selection
		# check move
		action = ParseActionType(GetActionOnTile(other_move[2], other_move[3]))
		if action != ActionType.Move:
			MoveValidationResult.rpc(false) # invalid move
			if not under_retry_count():
				ForceDraw()
			return
		# apply move
		__ApplyMove(other_move[2], other_move[3])
		MoveValidationResult.rpc(true)
	return

@rpc("any_peer")
func MoveValidationResult(result:bool) -> void:
	if not IS_LOCAL and not RPC_avalibile and call_state_ctrl != RPC_STATE.MoveV_RESULT: return
	RPC_avalibile = false
	# input validation
	var valid_move:bool = false
	if result is not bool:
		print("result is ", result)
	else:
		# read response
		valid_move = result
	# if move did not pass validation, ask for a different move
	if not valid_move:
		if not IS_LOCAL: validation_label.text = "Move was invalid"
		move_validation.emit(false)
		ClearCurrentSelection()
		if not under_retry_count():
			ForceDraw()
		return
	# move passed validation
	if not IS_LOCAL: validation_label.text = "Move was valid"
	move_validation.emit(true)
	var _move:Array[int] = [move_data[0].hex_to_int(), move_data[1].hex_to_int(), move_data[2].hex_to_int(), move_data[3].hex_to_int()]
	__ApplyMove(_move[2], _move[3])
	return

func AskForMoveValidation(move:String) -> void:
	# Function for the current player to ask the other player to validate their move
	if IS_LOCAL:
		move_data = CurrentSelection + move
		# send to self
		ProcessMoveRequest(move_data)
	else:
		validation_label = validation_relay.activate_validation_pannel()
		validation_label.text = "Sending move to opponent..."
		move_data = CurrentSelection + move
		# send to opponent
		call_state_ctrl = RPC_STATE.MoveV_RESULT # ready for reply
		RPC_avalibile = true
		ProcessMoveRequest.rpc(move_data)

func isActivePlayerTurn() -> bool:
	# check if current turn is the this player's turn
	if IS_LOCAL:
		return true
	else:
		return IsWhitePlayer == (round_num % 2 == 0)

func ReturnGameState() -> GameState:
	var value:int = GetGameState()
	#print("Game state is ", (value + 1) as GameState)
	return (value + 1) as GameState
	
func ClearHighlights() -> void:
	var x:int = 6
	var s:int = 0
	for i in 11:
		for j in x:
			SetTileHighlight(ActionType.NoAction, i, j+s, round_num%2)
		if i >= 5:
			x -= 1
			s += 1
		else:
			x += 1

func UpdateBoard() -> void:
	var x:int = 6
	var s:int = 0
	for i in 11:
		for j in x:
			# get board state at coordinates
			#print("--- update :: ", i, j+s)
			var piece:ChessPiece = ParsePieceType(GetPieceOnTile(i, j + s))
			# update piece on board at location
			#print("---- update with piece :: ", piece.player, " ", piece.type)
			SetPieceObjectOnTile(piece, i, j+s)
			var action:ActionType = ParseActionType(GetActionOnTile(i, j + s))
			#print("----- update with action :: ", action)
			# update tile highlight at location
			SetTileHighlight(action, i, j+s, round_num%2)
		# files grow by one on the end before rank 5,
		# and shrink by one at the start after rank 5.
		if i >= 5:
			x -= 1
			s += 1
		else:
			x += 1

func GetCurrentPieceObjectOnTile(rank:int, file:int) -> ChessPiece:
	# lookup piece on game board
	return hexboard.get_tile(rank, file).piece

func SetPieceObjectOnTile(piece:ChessPiece, rank:int, file:int) -> void:
	#print("setting piece :: t%s p%s r%s f%s" % [piece.type, piece.player, rank, file])
	if piece == GetCurrentPieceObjectOnTile(rank, file):
		return # skip if no change
	# update piece data
	hexboard.get_tile(rank, file).piece = piece
	# update piece mesh
	# clear piece on tile
	if (hexboard.get_tile(rank, file).instance != null):
		hexboard.get_tile(rank, file).instance.queue_free()
	if piece.type != PieceType.NoPiece:
		if piece.player:
			# set new piece on tile
			hexboard.get_tile(rank, file).instance = WhitePieces[piece.type].instantiate()
		else:
			# set new piece on tile
			hexboard.get_tile(rank, file).instance = BlackPieces[piece.type].instantiate()
		# set location
		hexboard.get_tile(rank, file).tile_instance.add_child(hexboard.get_tile(rank, file).instance)
	

func SetTileHighlight(action:ActionType, rank:int, file:int, color:int) -> void:
	var highlight_allowed:bool = isActivePlayerTurn() # don't highlight on inactive player's turn
	var tmp:TileInteraction = hexboard.get_tile(rank, file).tile_instance as TileInteraction
	# set new Highlight
	tmp.current_obj.hide()
	tmp.SetSelectionColor(color)
	if highlight_allowed and action == ActionType.Selectable:
		#tmp.select_obj.process_mode = Node.PROCESS_MODE_INHERIT
		#tmp.move_obj.process_mode = Node.PROCESS_MODE_DISABLED
		tmp.select_obj.show()
		tmp.move_obj.hide()
	elif highlight_allowed and action == ActionType.Move:
		#tmp.select_obj.process_mode = Node.PROCESS_MODE_DISABLED
		#tmp.move_obj.process_mode = Node.PROCESS_MODE_INHERIT
		tmp.select_obj.hide()
		tmp.move_obj.show()
	elif highlight_allowed and action == ActionType.MoveAndSelect:
		#tmp.select_obj.process_mode = Node.PROCESS_MODE_INHERIT
		#tmp.move_obj.process_mode = Node.PROCESS_MODE_INHERIT
		tmp.select_obj.show()
		tmp.move_obj.show()
	else:
		#tmp.select_obj.process_mode = Node.PROCESS_MODE_DISABLED
		#tmp.move_obj.process_mode = Node.PROCESS_MODE_DISABLED
		tmp.select_obj.hide()
		tmp.move_obj.hide()

func ClearCurrentSelection() -> void:
	disable_undo_button.emit()
	play_clank_sound.emit(1)
	CurrentSelection = ""
	ClearSelection() # remove current selection
	GetSelectableTiles() # update selectable tiles
	UpdateBoard()

func OnTileClicked(rank:int, file:int) -> void:
	if menu_active > 0 or not isActivePlayerTurn():
		return # ignore tile click if a menu is active or it is not this player's turn
	print("Tile Clicked :: rank=%d file=%d" % [rank, file])
	# verify coordinates are within board range
	if rank > 10 or rank < 0:
		print("ERROR:: rank not in range :: ", rank)
	if file > 10 or file < 0:
		print("ERROR:: file not in range :: ", file)
	# get the tile action type
	var action:ActionType = ParseActionType(GetActionOnTile(rank, file))
	if action == ActionType.NoAction:
		return
	elif action == ActionType.Selectable:
		# update Selection String
		CurrentSelection = "%x%x" % [rank, file]
		SetUndoSelectionButtonLocation(rank, file)
		play_clank_sound.emit(1)
		# update tiles with moves for selectable
		GetMoveTiles(rank, file) # notify driver of piece selection
		UpdateBoard()
		# update current selection
		var tmp:TileInteraction = hexboard.get_tile(rank, file).tile_instance as TileInteraction
		tmp.current_obj.show()
	elif action == ActionType.Move:
		# ask other player to validate move selection
		AskForMoveValidation("%x%x" % [rank, file])
		disable_undo_button.emit()
		return
	else:
		# tile has both a selection and a move
		# TODO
		print("NOT IMPLEMENTED :: Tile has both selection and move types!")

func __ApplyMove(rank:int, file:int) -> void:
	play_clank_sound.emit()
	# notify driver of move
	MovePiece(rank, file)
	call_state_ctrl = RPC_STATE.NoAction
	UpdateBoard()
	RoundCleanup() # round over, setup for next round
	if GetPromotionTile() != 0:
		if isActivePlayerTurn():
			activate_promotion.emit()
			menu_active += 1
		else:
			call_state_ctrl = RPC_STATE.Promotion
			RPC_avalibile = true
			# ready for auto promotion
	else:
		round_in_process = false

func SetUndoSelectionButtonLocation(rank:int, file:int) -> void:
	# https://forum.godotengine.org/t/how-to-tie-a-ui-node-to-a-3d-node/37292/3
	var pos_3d := hexboard.get_tile(rank, file).tile_instance.global_position + undo_button_offset
	var cam := get_viewport().get_camera_3d()
	var pos_2d := cam.unproject_position(pos_3d)
	move_selection_button.emit(pos_2d)

func ParsePieceType(piece:int) -> ChessPiece:
	if piece == 0:
		# no piece
		return ChessPiece.new(false, PieceType.NoPiece)
	elif piece < 7:
		# white piece
		return ChessPiece.new(true, ParsePieceTypeNoPlayer(piece - 1))
	else:
		# black piece
		return ChessPiece.new(false, ParsePieceTypeNoPlayer(piece - 7))

func ParsePieceTypeNoPlayer(piece:int) -> PieceType:
	match piece:
		0:
			return PieceType.King
		1:
			return PieceType.Queen
		2:
			return PieceType.Rook
		3:
			return PieceType.Bishop
		4:
			return PieceType.Knight
		5:
			return PieceType.Pawn
		_:
			return PieceType.NoPiece

func ParseActionType(action:int) -> ActionType:
	match action:
		0:
			return ActionType.NoAction
		1:
			return ActionType.Selectable
		2:
			return ActionType.Move
		3:
			return ActionType.MoveAndSelect
		_:
			return ActionType.NoAction

@rpc("any_peer")
func sync_promotion(selection:int) -> void:
	if not IS_LOCAL and not RPC_avalibile and call_state_ctrl != RPC_STATE.Promotion: return
	RPC_avalibile = false
	# input validation
	if selection is not int or selection > 4 or selection < 1: return
	RunPromotion(selection)
	round_in_process = false
	call_state_ctrl = RPC_STATE.NoAction

func _on_ui_promotion_selected(selection: int) -> void:
	play_clank_sound.emit(1)
	if DEMO_MODE: await get_tree().create_timer(DEMO_TIMER).timeout
	RunPromotion(selection)
	sync_promotion.rpc(selection)
	menu_active -= 1
	round_in_process = false
