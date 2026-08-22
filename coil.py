import pcbnew

def generate_perfect_nfc_footprint_graphics():
    board = pcbnew.GetBoard()
    
    # CONFIG:
    target_outer_width = 84.0
    target_outer_height = 53.0

    turns = 4
    track_w = 1
    spacing = 0.5

    center_x = 150.0
    center_y = 100.0
    
    width = target_outer_width - track_w
    height = target_outer_height - track_w
    step = track_w + spacing
    
    x_min = center_x - (width / 2.0)
    x_max = center_x + (width / 2.0)
    y_min = center_y - (height / 2.0)
    y_max = center_y + (height / 2.0)
    
    pts = [(x_min, y_min)]
    
    for i in range(turns):
        pts.append((x_max - i*step, y_min + i*step))
        pts.append((x_max - i*step, y_max - i*step))
        pts.append((x_min + i*step, y_max - i*step))
        pts.append((x_min + i*step, y_min + (i+1)*step))
        
    for i in range(len(pts) - 1):
        start = pcbnew.VECTOR2I_MM(pts[i][0], pts[i][1])
        end = pcbnew.VECTOR2I_MM(pts[i+1][0], pts[i+1][1])
        
        shape = pcbnew.PCB_SHAPE(board)
        shape.SetShape(pcbnew.SHAPE_T_SEGMENT)
        shape.SetStart(start)
        shape.SetEnd(end)
        shape.SetWidth(pcbnew.FromMM(track_w))
        shape.SetLayer(pcbnew.F_Cu)
        board.Add(shape)
        
    pcbnew.Refresh()
    print("COMPLETED")

generate_perfect_nfc_footprint_graphics()