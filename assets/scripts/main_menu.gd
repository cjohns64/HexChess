extends Control
@onready var root_panel: Panel = $Panel/RootPanel
@onready var game_type_panel: Panel = $Panel/GameTypePanel
@onready var connection_panel: Panel = $Panel/ConnectionPanel
@onready var option_button: OptionButton = $Panel/RootPanel/VBoxContainer/OptionButton
@onready var network_panel: Panel = $NetworkPanel
@onready var network_pannel_text: Label = $NetworkPanel/Text
@onready var network_pannel_button: Button = $NetworkPanel/Button
# ip address selection
@onready var ip_1_spin_box: SpinBox = $Panel/ip_config_panel/VBoxContainer/HBoxContainer/IP1_SpinBox
@onready var ip_2_spin_box: SpinBox = $Panel/ip_config_panel/VBoxContainer/HBoxContainer/IP2_SpinBox
@onready var ip_3_spin_box: SpinBox = $Panel/ip_config_panel/VBoxContainer/HBoxContainer/IP3_SpinBox
@onready var ip_4_spin_box: SpinBox = $Panel/ip_config_panel/VBoxContainer/HBoxContainer/IP4_SpinBox

@onready var ip_config_panel: Panel = $Panel/ip_config_panel

signal EndGame
signal StartLocal(num_windows:int)
signal StartOnline(server:bool, is_white_player:bool, ip_addr:String)

func _hide() -> void:
	$Panel.hide()

func _show() -> void:
	$Panel.show()

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
	ip_config_panel.show()
	connection_panel.hide()

func _on_network_panel_button_pressed() -> void:
	EndGame.emit()

func _on_ip_config_button_pressed() -> void:
	var ip_addr:String = "%d.%d.%d.%d" % [ip_1_spin_box.value, ip_2_spin_box.value, ip_3_spin_box.value, ip_4_spin_box.value]
	ip_config_panel.hide()
	StartOnline.emit(false, option_button.selected == 0, ip_addr)
