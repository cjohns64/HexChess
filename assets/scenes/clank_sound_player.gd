extends Node3D

@onready var stream_player_normal:AudioStreamPlayer3D = $NormalClanks

func play_sound() -> void:
	stream_player_normal.play()
