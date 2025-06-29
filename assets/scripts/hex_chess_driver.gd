extends GDHexChessDriver
class_name HexChess

signal gameOver(state:GameState, isWhiteTurn:bool)
signal activate_promotion()
signal play_clank_sound(type:int)
signal turn_changed(is_white_turn:bool)
enum ActionType {NoAction, Selectable, Move, MoveAndSelect}
enum PieceType {King, Queen, Rook, Bishop, Knight, Pawn, NoPiece}
enum GameState {Running, Checkmate, Stalemate, DeadPosistion, ThreefoldRepitition, FiftyMoveRule}
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


class Chessboard:
	var _internal_array: Array[TileObject] = []
	func _init(root:HexChess):
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
				tile_script.tile_clicked.connect(func(rank:int, file:int): root.OnTileClicked(rank, file))
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
var root_node:HexChess

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
	root_node = $"."
	hexboard = Chessboard.new(root_node)
	RoundSetup() # declare a new round
	GetSelectableTiles()
	UpdateBoard()

var round_in_process:bool = true
var game_over:bool = false
var menu_active:bool = false
var round_num:int = 0 # _ready will run the first round
func _process(delta: float) -> void:
	if !game_over and !round_in_process:
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
			menu_active = true
			ClearHighlights()
			#print("GAME OVER")
			gameOver.emit(ReturnGameState(), round_num % 2 == 0)

func ReturnGameState() -> GameState:
	var value:int = GetGameState()
	#print("Game state is ", (value + 1) as GameState)
	return (value + 1) as GameState
	
func ClearHighlights() -> void:
	var x:int = 6
	var s:int = 0
	for i in 11:
		for j in x:
			SetTileHighlight(ActionType.NoAction, i, j+s)
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
			SetTileHighlight(action, i, j+s)
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
	

func SetTileHighlight(action:ActionType, rank:int, file:int) -> void:
	var tmp:TileInteraction = hexboard.get_tile(rank, file).tile_instance as TileInteraction
	# set new Highlight
	tmp.current_obj.hide()
	if action == ActionType.Selectable:
		#tmp.select_obj.process_mode = Node.PROCESS_MODE_INHERIT
		#tmp.move_obj.process_mode = Node.PROCESS_MODE_DISABLED
		tmp.select_obj.show()
		tmp.move_obj.hide()
	elif action == ActionType.Move:
		#tmp.select_obj.process_mode = Node.PROCESS_MODE_DISABLED
		#tmp.move_obj.process_mode = Node.PROCESS_MODE_INHERIT
		tmp.select_obj.hide()
		tmp.move_obj.show()
	elif action == ActionType.MoveAndSelect:
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
	ClearSelection() # remove current selection
	GetSelectableTiles() # update selectable tiles
	UpdateBoard()

func OnTileClicked(rank:int, file:int) -> void:
	if menu_active:
		return # ignore tile click if a menu is active
	print("Tile Clicked :: rank=%d file=%d" % [rank, file])
	# verifiy coordinates are within board range
	if rank > 10 or rank < 0:
		print("ERROR:: rank not in range :: ", rank)
	if file > 10 or file < 0:
		print("ERROR:: file not in range :: ", file)
	# get the tile action type
	var action:ActionType = ParseActionType(GetActionOnTile(rank, file))
	if action == ActionType.NoAction:
		return
	elif action == ActionType.Selectable:
		SetUndoSelectionButtonLocation(rank, file)
		play_clank_sound.emit(1)
		# update tiles with moves for selectable
		GetMoveTiles(rank, file) # notify driver of piece selection
		UpdateBoard()
		# update current selection
		var tmp:TileInteraction = hexboard.get_tile(rank, file).tile_instance as TileInteraction
		tmp.current_obj.show()
	elif action == ActionType.Move:
		disable_undo_button.emit()
		play_clank_sound.emit()
		# notify driver of move
		MovePiece(rank, file)
		UpdateBoard()
		RoundCleanup() # round over, setup for next round
		if GetPromotionTile() != 0:
			activate_promotion.emit()
			menu_active = true
		else:
			round_in_process = false
	else:
		# tile has both a selection and a move
		# TODO
		print("NOT IMPLEMENTED :: Tile has both selection and move types!")

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

func _on_ui_promotion_selected(selection: int) -> void:
	play_clank_sound.emit(1)
	RunPromotion(selection)
	menu_active = false
	round_in_process = false
