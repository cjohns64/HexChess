extends Node3D

@onready var stream_player_normal:AudioStreamPlayer3D = $NormalClanks
@onready var stream_player_ui:AudioStreamPlayer3D = $UIClanks

func play_sound(type:int=0) -> void:
	if type == 0:
		stream_player_normal.play()
	else:
		stream_player_ui.play()
