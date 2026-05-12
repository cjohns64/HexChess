extends Control
@onready var root_panel: Panel = $Panel/RootPanel
@onready var game_type_panel: Panel = $Panel/GameTypePanel
@onready var connection_panel: Panel = $Panel/ConnectionPanel
@onready var option_button: OptionButton = $Panel/RootPanel/VBoxContainer/OptionButton
signal StartLocal(num_windows:int)
signal StartOnline(server:bool, is_white_player:bool)

func _ready() -> void:
	init_menu()

func init_menu() -> void:
	root_panel.show()
	game_type_panel.hide()
	connection_panel.hide()

func _on_new_game_button_pressed() -> void:
	root_panel.hide()
	game_type_panel.show()

func _on_local_button_pressed() -> void:
	# start a game on localhost with the one-window game world
	StartLocal.emit(1)

func _on_local_button_2window_pressed() -> void:
	# start a game on localhost with two windows
	StartLocal.emit(2)

func _on_remote_button_pressed() -> void:
	connection_panel.show()
	game_type_panel.hide()

func _on_return_button_pressed() -> void:
	init_menu()

func _on_server_button_pressed() -> void:
	StartOnline.emit(true, option_button.selected == 0)

func _on_client_button_pressed() -> void:
	StartOnline.emit(false, option_button.selected == 0)
