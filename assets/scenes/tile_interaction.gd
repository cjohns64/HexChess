extends Node3D
class_name TileInteraction

# matirial override
@export var mat_override:Mat
enum Mat {White, Black, Grey}
@onready var mesh = $"Container/tile-container/tile-grey/Tile-applied"
var mat_set = {
	Mat.Black:preload("res://assets/material/Tile-Black.tres"),
	Mat.Grey:preload("res://assets/material/Tile-Grey.tres"),
	Mat.White:preload("res://assets/material/Tile-White.tres"),
}

# clicked signal
@onready var click_area = $Container/Area3D
var rank:int = 0
var file:int = 0
signal tile_clicked(rank:int, file:int)

# selectable/moveable
@onready var select_obj = $"Container/selection-highlight"
@onready var move_obj = $"Container/movement-highlight"
@onready var current_obj = $"Container/current-highlight"

func _ready() -> void:
	SetMaterial(mat_override)
	click_area.input_ray_pickable = true

func SetMaterial(color:Mat) -> void:
	mesh.material_override = mat_set[color]
	
func PrintStats(text:String="") -> void:
	print("%s%s rank=%d file=%d" % [text, name, rank, file])

func _on_area_3d_input_event(camera: Node, event: InputEvent, event_position: Vector3, normal: Vector3, shape_idx: int) -> void:
	if event.is_action_pressed("mouseInteract"):
		#PrintStats("OnClicked Event Triggered::")
		tile_clicked.emit(rank, file) # new signal with rank and file info
