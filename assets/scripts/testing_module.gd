extends Node

var world:Node3D
var timer:float = 0
var TIME_MAX:float = 5

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	world = self.get_parent()

# rpc naturally manages type consistency
func _process(_delta: float) -> void:
	timer = timer + _delta
	if timer > TIME_MAX:
		timer = 0
		match world.hex_chess_driver.call_state_ctrl:
			world.hex_chess_driver.RPC_STATE.MoveV_PROCESS:
				print("Testing: MoveV_PROCESS")
				# test multiple valid RPC
				world.hex_chess_driver.ProcessMoveRequest.rpc("2343")
				world.hex_chess_driver.ProcessMoveRequest.rpc("7555")
				# invalid RPC
				world.hex_chess_driver.ProcessMoveRequest.rpc('c')
				#world.hex_chess_driver.ProcessMoveRequest.rpc([7,7,5,5])
				__full_test_outofturn()
			world.hex_chess_driver.RPC_STATE.MoveV_RESULT:
				print("Testing: MoveV_RESULT")
				# test multiple valid RPC
				world.hex_chess_driver.MoveValidationResult.rpc(true)
				world.hex_chess_driver.MoveValidationResult.rpc(true)
				# invalid RPC
				#world.hex_chess_driver.MoveValidationResult.rpc('c')
				__full_test_outofturn()
			world.hex_chess_driver.RPC_STATE.Promotion:
				print("Testing: Promotion")
				# test multiple valid RPC
				world.hex_chess_driver.sync_promotion.rpc(1)
				world.hex_chess_driver.sync_promotion.rpc(2)
				# invalid RPC
				#world.hex_chess_driver.sync_promotion.rpc('c')
				__full_test_outofturn()
			_: # RPC_STATE.NoAction
				print("Testing: NoAction")
				__full_test_outofturn()

func __full_test_outofturn() -> void:
	world.hex_chess_driver.sync_promotion.rpc(2)
	world.hex_chess_driver.ProcessMoveRequest.rpc("1155")
	world.hex_chess_driver.MoveValidationResult.rpc(true)
