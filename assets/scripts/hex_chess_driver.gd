extends HexChessDriver

enum ActionType {NoAction, Selectable, Move, MoveAndSelect}
enum PieceType {King, Queen, Rook, Bishop, Knight, Pawn, NoPiece}
enum GameState {Running, Checkmate, Stalemate, DeadPosistion, ThreefoldRepitition, FiftyMoveRule}
var WhitePieces = {
	PieceType.King:"res://assets/scenes/pieces/King-White.tscn",
	PieceType.Queen:"res://assets/scenes/pieces/Queen-White.tscn",
	PieceType.Rook:"res://assets/scenes/pieces/Rook-White.tscn",
	PieceType.Bishop:"res://assets/scenes/pieces/Bishop-White.tscn",
	PieceType.Knight:"res://assets/scenes/pieces/Knight-White.tscn",
	PieceType.Pawn:"res://assets/scenes/pieces/Pawn-White.tscn"
	}
var BlackPieces = {
	PieceType.King:"res://assets/scenes/pieces/King-Black.tscn",
	PieceType.Queen:"res://assets/scenes/pieces/Queen-Black.tscn",
	PieceType.Rook:"res://assets/scenes/pieces/Rook-Black.tscn",
	PieceType.Bishop:"res://assets/scenes/pieces/Bishop-Black.tscn",
	PieceType.Knight:"res://assets/scenes/pieces/Knight-Black.tscn",
	PieceType.Pawn:"res://assets/scenes/pieces/Pawn-Black.tscn"
	}

class ChessPiece:
	var player
	var type
	func _init(_is_white_player:bool, _type:PieceType) -> void:
		player = _is_white_player
		type = _type

func _ready() -> void:
	RoundSetup() # declare a new round
	GetSelectableTiles()
	UpdateBoard()

func ReturnGameState() -> GameState:
	var value:int = GetGameState()
	return (value + 1) as GameState

func UpdateBoard() -> void:
	var x:int = 6
	var s:int = 0
	for i in 11:
		for j in x:
			# get board state at coordinates
			var piece:ChessPiece = ParsePieceType(GetPieceOnTile(i, j + s))
			# update piece on board at location
			SetPieceObjectOnTile(piece, i, j+s)
			var action:ActionType = ParseActionType(GetActionOnTile(i, j + s))
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
	# TODO lookup piece on game board
	return ChessPiece.new(true, PieceType.King)

func SetPieceObjectOnTile(piece:ChessPiece, rank:int, file:int) -> void:
	if piece == GetCurrentPieceObjectOnTile(rank, file):
		return # skip if no change
	if piece.player:
		# white player
		var piece_list = WhitePieces
	else:
		# black player
		var piec_list = BlackPieces
	# clear piece on tile
	# TODO
	# set new piece on tile
	# TODO

func SetTileHighlight(action:ActionType, rank:int, file:int) -> void:
	# TODO clear Highlight
	# TODO set new Highlight
	pass

func ClearCurrentSelection() -> void:
	ClearSelection() # remove current selection
	GetSelectableTiles() # update selectable tiles
	UpdateBoard()

func OnTileClicked(rank:int, file:int) -> void:
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
		# update tiles with moves for selectable
		GetMoveTiles(rank, file) # notify driver of piece selection
		UpdateBoard()
	elif action == ActionType.Move:
		# notify driver of move
		MovePiece(rank, file)
		UpdateBoard()
		RoundCleanup() # round over, setup for next round
	else:
		# tile has both a selection and a move
		# TODO
		print("NOT IMPLEMENTED :: Tile has both selection and move types!")

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
