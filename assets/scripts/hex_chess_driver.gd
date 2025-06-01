extends HexChessDriver

enum ActionType {NoAction, Selectable, Move, MoveAndSelect}
enum PieceType {King, Queen, Rook, Bishop, Knight, Pawn, NoPiece}

class ChessPiece:
	var player
	var type
	func _init(_is_white_player:bool, _type:PieceType) -> void:
		player = _is_white_player
		type = _type

func _ready() -> void:
	pass
	
func UpdateBoard() -> void:
	var x:int = 6
	var s:int = 0
	for i in 11:
		for j in x:
			# get board state at coordinates
			var piece:ChessPiece = ParsePieceType(GetPieceOnTile(i, j))
			# TODO update piece on board at location
			var action:ActionType = ParseActionType(GetActionOnTile(i, j))
			# TODO update tile highlight at location
			
		if i >= 5:
			x -= 1
			s += 1
		else:
			x += 1
	
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
		GetMoveTiles(rank, file)
	elif action == ActionType.Move:
		pass
	else:
		pass
	
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
