extends Control
@onready var root_panel: Panel = $Panel/RootPanel
@onready var game_type_panel: Panel = $Panel/GameTypePanel
@onready var connection_panel: Panel = $Panel/ConnectionPanel
#@onready var option_button: OptionButton = $Panel/RootPanel/VBoxContainer/OptionButton
@onready var network_panel: Panel = $NetworkPanel
@onready var network_pannel_text: Label = $NetworkPanel/Text
@onready var network_pannel_button: Button = $NetworkPanel/Button
# ip address selection
@onready var ip_1_spin_box: SpinBox = $Panel/ip_config_panel/VBoxContainer/HBoxContainer/IP1_SpinBox
@onready var ip_2_spin_box: SpinBox = $Panel/ip_config_panel/VBoxContainer/HBoxContainer/IP2_SpinBox
@onready var ip_3_spin_box: SpinBox = $Panel/ip_config_panel/VBoxContainer/HBoxContainer/IP3_SpinBox
@onready var ip_4_spin_box: SpinBox = $Panel/ip_config_panel/VBoxContainer/HBoxContainer/IP4_SpinBox
# ip address selection panel
@onready var ip_config_panel: Panel = $Panel/ip_config_panel
# server config panel
@onready var server_config_panel: Panel = $Panel/server_config_panel
@onready var color_options: OptionButton = $Panel/server_config_panel/VBoxContainer/ColorOptions

signal EndGame
signal StartLocal(is_white_player:bool)
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
	# start a game in local mode with the one-window game world
	StartLocal.emit(color_options.selected == 0)

func _on_remote_button_pressed() -> void:
	connection_panel.show()
	game_type_panel.hide()

func _on_return_button_pressed() -> void:
	init_menu()

func _on_server_button_pressed() -> void:
	server_config_panel.show()
	connection_panel.hide()

func _on_client_button_pressed() -> void:
	ip_config_panel.show()
	connection_panel.hide()

func _on_network_panel_button_pressed() -> void:
	EndGame.emit()

func _on_ip_config_button_pressed() -> void:
	var ip_addr:String = "%d.%d.%d.%d" % [ip_1_spin_box.value, ip_2_spin_box.value, ip_3_spin_box.value, ip_4_spin_box.value]
	ip_config_panel.hide()
	StartOnline.emit(false, false, ip_addr)

func _on_server_config_button_pressed() -> void:
	StartOnline.emit(true, color_options.selected == 0)
	server_config_panel.hide()

signal TESTING_MODULE()
func _on_testing_button_pressed() -> void:
	# launches a server and also the testing module
	_on_server_config_button_pressed()
	TESTING_MODULE.emit()
