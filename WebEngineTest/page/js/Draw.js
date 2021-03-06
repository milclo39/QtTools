var ITEM_TYPE = {
    ZOOM : "zoom",
    SELECT : "select",
    ERASER : "eraser",
    LINE_FREE_PEN : "line_free_pen",
    LINE_FREE_MARKER : "line_free_marker",
    LINE_STRAIGHT : "line_straight",
    RECT_FILL : "rect_fill",
    RECT_EMPTY : "rect_empty",
    RECT_SELECT : "rect_select",
    TEXT : "text",
    IMAGE : "image",
};

//共通定義
var angle_rotation = 0; //回転
var scale_zoom = 1.0; //拡大
var background_width = $("#draw").width(); //背景幅
var background_height = $("#draw").height();　//背景高さ

//フリーズ
var is_freeze = false; //フリーズ
var is_freeze_draw = false; //フリーズ(描画)

//マスク
var item_mask = null;
var is_mask = false;
var is_draw_start_over_mask = false; //マスク上から描き始めたかの判定用

//ハイライト
var item_highlight = null;
var is_highlight = false;
var is_draw_start_over_highlight = false; //ハイライト上から描き始めたかの判定用

//コンペア
var is_compare = false;

//ズーム
var defaultX = 0;
var defaultY = 0;
var rangeX = 0;
var rangeY = 0;

//キャンバス
var canvas = new fabric.Canvas('canvas', {
    hoverCursor: 'pointer',
    targetFindTolerance: 15, //選択可能とする余白
    selection: false,
    preserveObjectStacking: true, //
//    perPixelTargetFind: true, //trueにするとピクセル単位であたり判定
});
canvas.setWidth($("#draw").width());
canvas.setHeight($("#draw").height());
canvas.zoomToPoint(new fabric.Point(($("#draw").width()) / 2, ($("#draw").height()) / 2), 1.0);

//描画関連
var item_type = ITEM_TYPE.LINE_FREE_PEN; //描画タイプ
var item = null; //描画アイテム
var isDown, origX, origY; //押下,押下時座標
var text_input = ""; //テキスト入力

//自由線設定
canvas.freeDrawingBrush.color = "rgba(0,0,0,1.0)"; //自由線色
canvas.freeDrawingBrush.width = 2 * 5; //自由線幅
canvas.backgroundColor = "rgba(0,0,0,0)"; //背景色
canvas.renderAll(); //描画
setItemType(ITEM_TYPE.LINE_FREE_PEN);

//選択設定
fabric.Object.prototype.set({ //選択表示設定
    transparentCorners: false,
    cornerColor: 'rgba(255,0,0,0.8)',
    borderColor: '#000000',
    cornerSize: 20,
    padding: 5
});

//ピンチ
var touchstart_bar = 0;
var touchmove_bar = 0;

//ハイライト
var item_highlight_bg_t = new fabric.Rect({
    left: 0,
    top: 0,
    originX: 'left',
    originY: 'top',
    width: canvas.width,
    height: canvas.height,
    angle: 0,
    strokeWidth : 0,
    selectable: false
});
var item_highlight_bg_b = new fabric.Rect({
    left: 0,
    top: 0,
    originX: 'left',
    originY: 'top',
    width: canvas.width,
    height: canvas.height,
    angle: 0,
    strokeWidth : 0,
    selectable: false
});
var item_highlight_bg_l = new fabric.Rect({
    left: 0,
    top: 0,
    originX: 'left',
    originY: 'top',
    width: canvas.width,
    height: canvas.height,
    angle: 0,
    strokeWidth : 0,
    selectable: false
});
var item_highlight_bg_r = new fabric.Rect({
    left: 0,
    top: 0,
    originX: 'left',
    originY: 'top',
    width: canvas.width,
    height: canvas.height,
    angle: 0,
    strokeWidth : 0,
    selectable: false
});

//マルチページ
var objs_camera = []; //カメラ背景 オブジェクトリスト
var objs_wb1 = []; //ホワイトボード1 オブジェクトリスト
var objs_wb2 = []; //ホワイトボード2 オブジェクトリスト
var objs_wb3 = []; //ホワイトボード3 オブジェクトリスト
var objs_wb4 = []; //ホワイトボード4 オブジェクトリスト
var objs_transparent = []; //透明背景 オブジェクトリスト
var objs_image = []; //静止画 オブジェクトリスト
var objs_video = []; //動画 オブジェクトリスト
function clearObjectsCanvas(type) {
    if (type == "camera") {
        objs_camera = [];
    } else if (type == "whiteboard1") {
        objs_wb1 = [];
    } else if (type == "whiteboard2") {
        objs_wb2 = [];
    } else if (type == "whiteboard3") {
        objs_wb3 = [];
    } else if (type == "whiteboard4") {
        objs_wb4 = [];
    } else if (type == "transparent") {
        objs_transparent = [];
    } else if (type == "image") {
        objs_image = [];
    } else if (type == "video") {
        objs_video = [];
    }
}
function saveObjectsCanvas(type) {
    for (var i = 0; i < canvas._objects.length; i++) {
        if (type == "camera") {
            objs_camera.push(canvas._objects[i]);
        } else if (type == "whiteboard1") {
            objs_wb1.push(canvas._objects[i]);
        } else if (type == "whiteboard2") {
            objs_wb2.push(canvas._objects[i]);
        } else if (type == "whiteboard3") {
            objs_wb3.push(canvas._objects[i]);
        } else if (type == "whiteboard4") {
            objs_wb4.push(canvas._objects[i]);
        } else if (type == "transparent") {
            objs_transparent.push(canvas._objects[i]);
        } else if (type == "image") {
            objs_image.push(canvas._objects[i]);
        } else if (type == "video") {
            objs_video.push(canvas._objects[i]);
        }
    }
}
function loadObjectsCanvas(type) {
    if (type == "camera") {
        for (var i = 0; i < objs_camera.length; i++) canvas.add(objs_camera[i]);
    } else if (type == "whiteboard1") {
        for (var i = 0; i < objs_wb1.length; i++) canvas.add(objs_wb1[i]);
    } else if (type == "whiteboard2") {
        for (var i = 0; i < objs_wb2.length; i++) canvas.add(objs_wb2[i]);
    } else if (type == "whiteboard3") {
        for (var i = 0; i < objs_wb3.length; i++) canvas.add(objs_wb3[i]);
    } else if (type == "whiteboard4") {
        for (var i = 0; i < objs_wb4.length; i++) canvas.add(objs_wb4[i]);
    } else if (type == "transparent") {
        for (var i = 0; i < objs_transparent.length; i++) canvas.add(objs_transparent[i]);
    } else if (type == "image") {
        for (var i = 0; i < objs_image.length; i++) canvas.add(objs_image[i]);
    } else if (type == "video") {
        for (var i = 0; i < objs_video.length; i++) canvas.add(objs_video[i]);
    }
    canvas.renderAll(); //更新
}

//アンドゥ/リドゥ
var isRedoing = false;
var isUndoing = false;
var listRedo = [];
var listUndo = [];
canvas.on('object:added',function() {
    if(!isRedoing) {
        listRedo = [];
    }
    if(!isUndoing) {
        listUndo = [];
    }
    isRedoing = false;
    isUndoing = false;
});
function undo() {
    if (listUndo.length > 0) {
        isUndoing = true;
        canvas.add(listUndo.pop());
        canvas.renderAll(); //更新
    } else if (canvas._objects.length > 0) {
        listRedo.push(canvas._objects.pop());
        canvas.renderAll(); //更新
    }
}
function redo() {
    if(listRedo.length > 0) {
        isRedoing = true;
        canvas.add(listRedo.pop());
        canvas.renderAll(); //更新
    }
}

function selectObject(){
    if(null !== canvas.getActiveObject() && undefined !== canvas.getActiveObject()){
        return true
    }
}

function objectCopy(){
    canvas.getActiveObject().clone(function(cloned) {
        cloned.clone(function(clonedObj) {
            canvas.discardActiveObject();
            clonedObj.set({
                              left: clonedObj.left + 10,
                              top: clonedObj.top + 10,
                              evented: true,
                          });
            if (clonedObj.type === 'activeSelection') {
                // active selection needs a reference to the canvas.
                clonedObj.canvas = canvas;
                clonedObj.forEachObject(function(obj) {
                    canvas.add(obj);
                });
                // this should solve the unselectability
                clonedObj.setCoords();
            } else {
                canvas.add(clonedObj);
            }
            cloned.top += 10;
            cloned.left += 10;
            canvas.setActiveObject(clonedObj);
            canvas.requestRenderAll();
        });
    });
}

function objectForward(){
    canvas.bringForward(canvas.getActiveObject()); //一つ手前
    canvas.renderAll(); //更新
}

function objectBackword(){
    canvas.sendBackwards(canvas.getActiveObject()); //一つ奥
    canvas.renderAll(); //更新
}

//背景
function setBackgroung(path, angle) {
    canvas.backgroundColor = "rgba(255, 255, 255, 1.0)";
    fabric.Image.fromURL(path, function(img) {
        var canvasAspect = canvas.width / canvas.height;
        var imgAspect = img.width / img.height;
        var left, top, scaleFactor;
        if (canvasAspect <= imgAspect) {
            scaleFactor = canvas.width / img.width;
            left = 0;
            top = -((img.height * scaleFactor) - canvas.height) / 2;
        } else {
            scaleFactor = canvas.height / img.height;
            top = 0;
            left = -((img.width * scaleFactor) - canvas.width) / 2;
        }
        var ang = 180;
        canvas.setBackgroundImage(img, canvas.renderAll.bind(canvas), {
            top: top,
            left: left,
            originX: angle == 0 ? 'left' : 'right',
            originY: angle == 0 ? 'top' : 'bottom',
            scaleX: scaleFactor,
            scaleY: scaleFactor,
            angle: angle,
        });
        canvas.renderAll(); //更新
    });
}

//タイプ設定
function setItemType(m) {
    if (m == ITEM_TYPE.ZOOM) {
        canvas.isDrawingMode = true;
        canvas.freeDrawingCursor = 'pointer';
        canvas.selection = false; //複数選択無効
        if (canvas.freeDrawingBrush.color.slice(-4) != "0.0)") {
            canvas.freeDrawingBrush.color = canvas.freeDrawingBrush.color.slice(0,-4) + "0.0)";
        }
        item_type = ITEM_TYPE.ZOOM;
    }else if (m == ITEM_TYPE.SELECT) {
        canvas.isDrawingMode = false;
//        canvas.defaultCursor = 'url(images/cursor_handopen.png) 15 15, auto';
        canvas.defaultCursor = 'pointer';
        canvas.selection = true; //複数選択有効
        item_type = ITEM_TYPE.SELECT;
    } else if (m == ITEM_TYPE.ERASER) {
        canvas.isDrawingMode = false;
        canvas.defaultCursor = 'url(images/cursor_eraser.png) 0 30, auto';
        canvas.selection = false; //複数選択無効
        item_type = ITEM_TYPE.ERASER;
        canvas.discardActiveObject();
        canvas.requestRenderAll();
    } else {
        canvas.isDrawingMode = true;
        canvas.selection = false; //複数選択無効
        if (m == ITEM_TYPE.LINE_FREE_PEN) {
            item_type = ITEM_TYPE.LINE_FREE_PEN;
            if (canvas.freeDrawingBrush.color.slice(-4) == "0.0)") {
                canvas.freeDrawingBrush.color = canvas.freeDrawingBrush.color.slice(0,-4) + "1.0)";
            }
            canvas.freeDrawingCursor = 'url(images/cursor_pencil.png) 0 30, auto';
        } else if (m == ITEM_TYPE.LINE_FREE_MARKER) {
            item_type = ITEM_TYPE.LINE_FREE_MARKER;
            if (canvas.freeDrawingBrush.color.slice(-4) == "0.0)") {
                canvas.freeDrawingBrush.color = canvas.freeDrawingBrush.color.slice(0,-4) + opacity_marker.toFixed(1) + ")";
            }
            canvas.freeDrawingCursor = 'url(images/cursor_marker.png) 0 30, auto';
        } else {
            if (canvas.freeDrawingBrush.color.slice(-4) != "0.0)") {
                canvas.freeDrawingBrush.color = canvas.freeDrawingBrush.color.slice(0,-4) + "0.0)";
            }
            if (m == ITEM_TYPE.LINE_STRAIGHT) {
                item_type = ITEM_TYPE.LINE_STRAIGHT;
                canvas.freeDrawingCursor = 'url(images/cursor_cross.png) 15 15, auto';
            } else if (m == ITEM_TYPE.RECT_FILL) {
                item_type = ITEM_TYPE.RECT_FILL;
                canvas.freeDrawingCursor = 'url(images/cursor_cross.png) 15 15, auto';
            } else if (m == ITEM_TYPE.RECT_EMPTY) {
                item_type = ITEM_TYPE.RECT_EMPTY;
                canvas.freeDrawingCursor = 'url(images/cursor_cross.png) 15 15, auto';
            } else if (m == ITEM_TYPE.RECT_SELECT) {
                item_type = ITEM_TYPE.RECT_SELECT;
                canvas.freeDrawingCursor = 'url(images/cursor_cross.png) 15 15, auto';
            } else if (m == ITEM_TYPE.TEXT) {
                item_type = ITEM_TYPE.TEXT;
                canvas.freeDrawingCursor = 'url(images/cursor_tag.png) 0 0, auto';
//                canvas.defaultCursor = 'pointer';
            } else if (m == ITEM_TYPE.IMAGE) {
//                canvas.isDrawingMode = false;
                item_type = ITEM_TYPE.IMAGE;
                canvas.freeDrawingCursor = 'url(images/cursor_pict_cross.png) 0 0, auto';
//                canvas.defaultCursor = 'pointer';
            }
        }
    }
//    if (item_type == ITEM_TYPE.IMAGE) {
//        canvas.perPixelTargetFind = false;
//    } else {
//        canvas.perPixelTargetFind = true;
//    }
}

function down(pointer) {
    if (item_type == ITEM_TYPE.ZOOM) {
        if (background_type == "camera" || background_type == "video") { //カメラ/動画
//ズーム中の描画検討
            defaultX = moveX_draw;
            defaultY = moveY_draw;
            rangeX = Math.ceil(($("#draw").width() * scale_zoom - $("#draw").width()) / 2);
            rangeY = Math.ceil(($("#draw").height() * scale_zoom - $("#draw").height()) / 2);
//ズーム中の描画検討
//            defaultX = parseInt($('#video').css('margin-left').replace('px', '')); //移動用に開始位置を記憶
//            defaultY = parseInt($('#video').css('margin-top').replace('px', ''));
//            rangeX = Math.ceil((background_width * scale_zoom - canvas.width) / 2);
//            rangeY = Math.ceil((background_height * scale_zoom - canvas.height) / 2);
        } else { //ホワイトボード/静止画
//ズーム中の描画検討
            defaultX = moveX_draw;
            defaultY = moveY_draw;
            rangeX = Math.ceil(($("#draw").width() * scale_zoom - $("#draw").width()) / 2);
            rangeY = Math.ceil(($("#draw").height() * scale_zoom - $("#draw").height()) / 2);
//ズーム中の描画検討

//            defaultX = parseInt($('#canvas').css('margin-left').replace('px', '')); //移動用に開始位置を記憶
//            defaultY = parseInt($('#canvas').css('margin-top').replace('px', ''));
//            rangeX = Math.ceil((background_width * scale_zoom - canvas.width) / 2);
//            rangeY = Math.ceil((background_height * scale_zoom - canvas.height) / 2);
        }
        return;
    }
    //カメラライブ中の場合はフリーズする
//    if (document.getElementById("video").style.display == "block" && is_freeze == false) {
//        freeze(); is_freeze_draw = true;
//    }
    edit_text = false; //前画像が更新されるのでテキスト入力無効にする

    if (item_type == ITEM_TYPE.LINE_STRAIGHT) {
        item = new fabric.Line([origX, origY, origX, origY], {
            strokeWidth: width_pen,
            fill: color_marker.slice(0,-4) + opacity_marker.toFixed(1) + ")",
            stroke: color_marker.slice(0,-4) + opacity_marker.toFixed(1) + ")",
            originX: 'left',
            originY: 'top',
        });
        canvas.add(item);
        canvas.renderAll(); //更新
    } else if (item_type == ITEM_TYPE.RECT_FILL) {
        item = new fabric.Rect({
            left: origX,
            top: origY,
            originX: 'left',
            originY: 'top',
            width: pointer.x-origX,
            height: pointer.y-origY,
            angle: 0,
            strokeWidth : width_pen,
            stroke: color_marker.slice(0,-4) + opacity_marker.toFixed(1) + ")",
            fill: color_marker.slice(0,-4) + opacity_marker.toFixed(1) + ")"
        });
        canvas.add(item);
        canvas.renderAll(); //更新
    } else if (item_type == ITEM_TYPE.RECT_EMPTY) {
        item = new fabric.Rect({
            left: origX,
            top: origY,
            originX: 'left',
            originY: 'top',
            width: pointer.x-origX,
            height: pointer.y-origY,
            angle: 0,
            strokeWidth : width_pen,
            stroke: color_pen,
            fill: 'rgba(0,0,0,0.0)'
        });
        canvas.add(item);
        canvas.renderAll(); //更新
    } else if (item_type == ITEM_TYPE.RECT_SELECT) {
        item = new fabric.Rect({
            left: origX,
            top: origY,
            originX: 'left',
            originY: 'top',
            width: pointer.x-origX,
            height: pointer.y-origY,
            angle: 0,
            strokeWidth : 10,
            stroke: 'rgba(0,0,0,1.0)',
            fill: 'rgba(0,0,0,0.0)',
            strokeDashArray: [12, 8],
        });
        canvas.add(item);
        canvas.renderAll(); //更新
    } else if (item_type == ITEM_TYPE.TEXT) {
        text_input = ""; //クリア
        item = new fabric.IText(text_input, {
            left: origX,
            top: origY,
            fontSize: font_size_text,
            fontFamily: font_family_text,
            fontWeight: font_weight_text,
            fill: color_text,
            backgroundColor: color_text_background,
        });
        canvas.add(item).setActiveObject(item);
        item.width = 200
        item.enterEditing()
        item.setCoords();
        item.hiddenTextarea.focus();
        canvas.renderAll(); //更新
        setItemType(ITEM_TYPE.SELECT)
    } else if (item_type == ITEM_TYPE.IMAGE) {
		canvas.isDrawingMode = false;
        fabric.Image.fromURL(imgInsertPath, function(img) {
            item = img.set({
                left: origX,
                top: origY,
            });
            item.scaleToWidth(100);
            item.scaleToHeight(100);
            canvas.add(item).setActiveObject(item);
            canvas.renderAll(); //更新
        });
    }
}

var moveX = 0, moveY = 0;
var moveX_draw = 0, moveY_draw = 0;
function move(pointer, o) {
//function move(pointer) {
    if (item_type == ITEM_TYPE.ZOOM) {
        moveX = parseInt(-(origX - pointer.x), 10);
        moveY = parseInt(-(origY - pointer.y), 10);
        moveX = defaultX + moveX;
        moveY = defaultY + moveY;
        if (background_type == "camera" || background_type == "video") { //カメラ/動画
//ズーム中の描画検討
            var is_updateX = false, is_updateY = false;
            if (-1 * rangeX <= moveX && rangeX >= moveX) {
                moveX_draw += o.e.movementX; is_updateX = true;
                $('#video').css('margin-left', moveX + 'px');
            }
            if (-1 * rangeY <= moveY && rangeY >= moveY) {
                $('#video').css('margin-top', moveY + 'px');
                moveY_draw += o.e.movementY; is_updateY = true;
            }
            if (is_updateX == true && is_updateY == true) {
                canvas.relativePan(new fabric.Point(o.e.movementX, o.e.movementY));
            } else if (is_updateX == true && is_updateY == false) {
                canvas.relativePan(new fabric.Point(o.e.movementX, 0));
            } else if (is_updateX == false && is_updateY == true) {
                canvas.relativePan(new fabric.Point(0, o.e.movementY));
            }
//ズーム中の描画検討


//            if (-1 * rangeX <= moveX && rangeX >= moveX) {
//                $('#video').css('margin-left', moveX + 'px');
//                $('#canvas').css('margin-left', moveX + 'px');
//            }
//            if (-1 * rangeY <= moveY && rangeY >= moveY) {
//                $('#video').css('margin-top', moveY + 'px');
//                $('#canvas').css('margin-top', moveY + 'px');
//            }
        } else { //ホワイトボード/静止画
//ズーム中の描画検討
            var is_updateX = false, is_updateY = false;
            if (-1 * rangeX <= moveX && rangeX >= moveX) {
                moveX_draw += o.e.movementX; is_updateX = true;
            }
            if (-1 * rangeY <= moveY && rangeY >= moveY) {
                moveY_draw += o.e.movementY; is_updateY = true;
            }
            if (is_updateX == true && is_updateY == true) {
                canvas.relativePan(new fabric.Point(o.e.movementX, o.e.movementY));
            } else if (is_updateX == true && is_updateY == false) {
                canvas.relativePan(new fabric.Point(o.e.movementX, 0));
            } else if (is_updateX == false && is_updateY == true) {
                canvas.relativePan(new fabric.Point(0, o.e.movementY));
            }

//            if (-1 * rangeX <= moveX && rangeX >= moveX) {
//                $('#canvas').css('margin-left', moveX + 'px');
//            }
//            if (-1 * rangeY <= moveY && rangeY >= moveY) {
//                $('#canvas').css('margin-top', moveY + 'px');
//            }
        }
    } else if (item_type == ITEM_TYPE.LINE_STRAIGHT) {
        item.set({ x2: pointer.x, y2: pointer.y });
        item.setCoords();
        canvas.renderAll(); //更新
    } else if (item_type == ITEM_TYPE.RECT_FILL || item_type == ITEM_TYPE.RECT_EMPTY || item_type == ITEM_TYPE.RECT_SELECT) {
        if(origX > pointer.x) {
            item.set({ left: Math.abs(pointer.x) });
        }
        if(origY > pointer.y) {
            item.set({ top: Math.abs(pointer.y) });
        }
        item.set({ width: Math.abs(origX - pointer.x) });
        item.set({ height: Math.abs(origY - pointer.y) });
        item.setCoords();
        canvas.renderAll(); //更新
    } else if (item_type == ITEM_TYPE.IMAGE) {
        item = canvas.getActiveObject()
        if (origX > pointer.x) {
            item.set({ left: Math.abs(origX) - Math.abs(origX - pointer.x), flipX: true });
        } else {
            item.set({ left: Math.abs(origX), flipX: false });
        }
        if (origY > pointer.y) {
            item.set({ top: Math.abs(origY) - Math.abs(origY - pointer.y), flipY: true });
        } else {
            item.set({ top: Math.abs(origY), flipY: false });
        }
        item.scaleToWidth(Math.abs(origX - pointer.x));
        item.scaleToHeight(Math.abs(origY - pointer.y));
        item.setCoords();
        canvas.renderAll(); //更新
    }
}
function up(pointer) {
    if (is_mask == true && canvas.isDrawingMode == true) {
        canvas._objects[canvas._objects.length - 1].selectable = false; //今描いたオブジェクトを選択不可にする
        canvas.renderAll();
        if (is_draw_start_over_mask == false) {
            canvas.sendToBack(canvas._objects[canvas._objects.length - 1]); //線を一番奥にする(マスク解除後に線は全て消すので順番は気にしない)
        }
        is_draw_start_over_mask = false;
    }
    if (is_highlight == true && canvas.isDrawingMode == true) {
        canvas._objects[canvas._objects.length - 1].selectable = false; //今描いたオブジェクトを選択不可にする
        canvas.renderAll();
        if (is_draw_start_over_highlight == true) {
            canvas.sendToBack(canvas._objects[canvas._objects.length - 1]); //線を一番奥にする(マスク解除後に線は全て消すので順番は気にしない)
        }
        is_draw_start_over_highlight = false;
    }
    if (item_type == ITEM_TYPE.ZOOM) {
//        if (document.getElementById("video").style.display != "none") {
//            document.getElementById('video').play(); //video再開
//        }
        return;
    }
//    if (is_freeze_draw == true) {
//        freeze(); is_freeze_draw = false;
//    }
    if (item_type == ITEM_TYPE.RECT_SELECT) { //範囲指定キャプチャ
        canvas.remove(item);
        canvas.renderAll(); //更新
        var finX = Math.abs(pointer.x);
        var finY = Math.abs(pointer.y);
        var x = origX < finX ? origX : finX;
        var y = origY < finY ? origY : finY;
        download_image(x, y, Math.abs(origX - finX), Math.abs(origY - finY));
        setItemType(ITEM_TYPE.LINE_FREE_PEN);
    }
    if (item_type == ITEM_TYPE.IMAGE) { //画像挿入
        setItemType(ITEM_TYPE.SELECT);
    }
}

var index_switch_1 = -1, index_switch_2 = -1;
var compare_item_scale_x = -1, compare_item_scale_y = -1;
canvas.on('object:moving', function(o) {
    if (list_item_compare.includes(o.target)) {
        if (compare_item_scale_x == -1 && compare_item_scale_y == -1) { //移動して最初の1回だけ実行
            o.target.opacity = 0.5;
            compare_item_scale_x = o.target.scaleX * 0.5;
            compare_item_scale_y = o.target.scaleY * 0.5;
            o.target.set({ scaleX: compare_item_scale_x, scaleY: compare_item_scale_y });
            canvas.bringToFront(o.target); //一番手前
        }

        var rect = o.e.target.getBoundingClientRect();
        var pointer = canvas.getPointer(o.e);
        if (o.e.clientX !== undefined) {
            pointer.x = o.e.clientX - rect.left;
            pointer.y = o.e.clientY - rect.top;
        } else {
            pointer.x = o.e.touches[0].clientX - rect.left;
            pointer.y = o.e.touches[0].clientY - rect.top;
        }
        o.target.set({ originX: 'center', originY: 'center', left: pointer.x, top: pointer.y });

        index_switch_1 = index_switch_2 = -1;
        o.target.setCoords();
        canvas.forEachObject(function(obj) {
            if (obj === o.target) return;
            if (o.target.isContainedWithinObject(obj) && list_item_compare.includes(obj)) { //画像内に収まった場合は入れ替え
                index_switch_1 = list_item_compare.indexOf(obj);
                index_switch_2 = list_item_compare.indexOf(o.target);
            }
        });
    }
});
canvas.on('object:modified', function(o) {
    if (list_item_compare.includes(o.target)) {
        o.target.opacity = 1.0;
        compare_item_scale_x = -1;
        compare_item_scale_y = -1;

        if (index_switch_1 != -1 && index_switch_2 != -1) {
            var img = list_image_compare[index_switch_1];
            list_image_compare.splice(index_switch_1, 1, list_image_compare[index_switch_2]);
            list_image_compare.splice(index_switch_2, 1, img);
            var is_check_1 = list_item_compare[index_switch_1].getObjects().length > 1 ? true : false;
            var is_check_2 = list_item_compare[index_switch_2].getObjects().length > 1 ? true : false;
            canvas.remove(list_item_compare[index_switch_1]);
            canvas.remove(list_item_compare[index_switch_2]);
            if (is_check_1 == true) list_selecteditem_compare.splice(list_selecteditem_compare.indexOf(list_item_compare[index_switch_1]), 1);
            if (is_check_2 == true) list_selecteditem_compare.splice(list_selecteditem_compare.indexOf(list_item_compare[index_switch_2]), 1);
            addImageItem(index_switch_1, list_image_compare.length, list_image_compare[index_switch_1], false, is_check_2);
            addImageItem(index_switch_2, list_image_compare.length, list_image_compare[index_switch_2], false, is_check_1);
            setTimeout(function() { index_switch_1 = index_switch_2 = -1; }, 1000);
        } else {
            relocateImageItem(list_image_compare.length);
        }
    }
});
canvas.on('mouse:over',function(o) {
    if (item_type == ITEM_TYPE.ERASER && isDown == true) { //部分消し
        remove_object(o.target);
    }
});
canvas.on('mouse:out', function(o) {
});
canvas.on('mouse:down', function(o) {
    isDown = true;
    if (o.target == item_mask && item_type != ITEM_TYPE.ZOOM && item_type != ITEM_TYPE.SELECT && item_type != ITEM_TYPE.ERASER) {
        is_draw_start_over_mask = true;
    }
    if (o.target == item_highlight && item_type != ITEM_TYPE.ZOOM && item_type != ITEM_TYPE.SELECT && item_type != ITEM_TYPE.ERASER) {
        is_draw_start_over_highlight = true;
    }
    var pointer = canvas.getPointer(o.e);
    if (item_type == ITEM_TYPE.ZOOM) {
        rangeX = pointer.x; //これでズームした上での位置が取れてそう....
        rangeY = pointer.y;
    }
    var rect = o.e.target.getBoundingClientRect();
    if (o.e.clientX !== undefined) {
        pointer.x = o.e.clientX - rect.left;
        pointer.y = o.e.clientY - rect.top;
    } else {
        pointer.x = o.e.touches[0].clientX - rect.left;
        pointer.y = o.e.touches[0].clientY - rect.top;
    }
    origX = pointer.x;
    origY = pointer.y;
    down(pointer);
    o.e.preventDefault(); //デフォルトイベントをキャンセル
});
canvas.on('mouse:move', function(o) {
    if (!isDown) return;
    var rect = o.e.target.getBoundingClientRect();
    var pointer = canvas.getPointer(o.e);
    if (o.e.clientX !== undefined) {
        pointer.x = o.e.clientX - rect.left;
        pointer.y = o.e.clientY - rect.top;
    } else {
        pointer.x = o.e.touches[0].clientX - rect.left;
        pointer.y = o.e.touches[0].clientY - rect.top;
    }
    if (o.target == item_highlight && is_highlight == true && item_type == ITEM_TYPE.SELECT) { //ハイライトの暗い部分調整
        var w = Math.ceil(item_highlight.width * item_highlight.scaleX);
        var h = Math.ceil(item_highlight.height * item_highlight.scaleY);
        var x_bg = canvas.width % -2 == 0 ? Math.ceil(canvas.width / -2) : canvas.width / -2;
        var y_bg = canvas.height % -2 == 0 ? Math.ceil(canvas.height / -2) : canvas.height / -2;
        item_highlight_bg_t.set({ clipTo: function (ctx) { ctx.rect(x_bg, y_bg, canvas.width, Math.ceil(item_highlight.top)); } });
        item_highlight_bg_b.set({ clipTo: function (ctx) { ctx.rect(x_bg, y_bg + Math.ceil(item_highlight.top) + h, canvas.width, canvas.height - Math.ceil(item_highlight.top) - h); } });
        item_highlight_bg_l.set({ clipTo: function (ctx) { ctx.rect(x_bg, y_bg + Math.ceil(item_highlight.top), item_highlight.left, h); } });
        item_highlight_bg_r.set({ clipTo: function (ctx) { ctx.rect(x_bg + item_highlight.left + w, y_bg + Math.ceil(item_highlight.top), canvas.width - item_highlight.left - w, h); } });
        canvas.renderAll(); //更新
    }
    move(pointer, o);
//    move(pointer);
    o.e.preventDefault(); //デフォルトイベントをキャンセル
});
canvas.on('mouse:up', function(o) {
    isDown = false;
    var pointer = canvas.getPointer(o.e);
    var rect = o.e.target.getBoundingClientRect();
    if (o.e.clientX !== undefined) {
        pointer.x = o.e.clientX - rect.left;
        pointer.y = o.e.clientY - rect.top;
    } else {
        //pointer.x = o.e.touches[0].clientX - rect.left;
        //pointer.y = o.e.touches[0].clientY - rect.top;
    }
    if (item_type != ITEM_TYPE.ZOOM && is_show_setting == true) { //メニューが表示されているなら非表示にする(拡大中はどうせ描けないので表示したままにする)
        hideSetting();
        if (Math.abs(origX - pointer.x) == 0 && Math.abs(origY - pointer.y) == 0 && (item_type == ITEM_TYPE.LINE_FREE_PEN || item_type == ITEM_TYPE.LINE_FREE_MARKER)) { //メニュー表示中に描画した線は消す
            canvas.remove(canvas._objects[canvas._objects.length -1]);
            canvas.renderAll(); //更新
        }
    }
    if (index_switch_1 == -1 && index_switch_2 == -1 && item_type == ITEM_TYPE.SELECT &&  list_item_compare.includes(o.target)) {
        var index = list_item_compare.indexOf(o.target);
        var item = list_item_compare[index];
        canvas.remove(item);
        if (item.getObjects().length == 1) {
            addImageItem(index, list_image_compare.length, list_image_compare[index], false, true);
        } else {
            list_selecteditem_compare.splice(list_selecteditem_compare.indexOf(item), 1);
            addImageItem(index, list_image_compare.length, list_image_compare[index], false, false);
        }
        canvas.renderAll(); //更新
    }
    up(pointer);
    o.e.preventDefault(); //デフォルトイベントをキャンセル
});
canvas.on('selection:created', function () {
    if (item_type != ITEM_TYPE.ERASER) return;
    remove_object(canvas.getActiveObject());
});

var item_type_tmp = "line_free_pen";
addEventListener("contextmenu", function(e){
    e.preventDefault(); //デフォルトイベントをキャンセル(メニューを表示しなくする)
}, false);
addEventListener("mousedown", function(e){
    switch (e.button) {
        case 1 :
        case 2 :
            isDown = true;
//            item_type_tmp = item_type;
//            setItemType(ITEM_TYPE.ERASER);
            break;
    }
}, false);
addEventListener("mouseup", function(e){
    switch (e.button) {
        case 1 :
        case 2 :
//            if (is_show_setting == true) {
//                hideSetting();
//            }
            isDown = false;
//            setItemType(item_type_tmp);
            break;
    }
}, false);
//addEventListener("keydown", function(e) {
//    if (item_type != ITEM_TYPE.TEXT) {
//        var is_focus_zoomrange = $('.zoomrange').is(':focus'); //ズームがアクティブか判定
//        if (e.keyCode == 13) { //Enter:複製
//            canvas.getActiveObject().clone(function(cloned) {
//                cloned.clone(function(clonedObj) {
//                    canvas.discardActiveObject();
//                    clonedObj.set({
//                        left: clonedObj.left + 10,
//                        top: clonedObj.top + 10,
//                        evented: true,
//                    });
//                    if (clonedObj.type === 'activeSelection') {
//                        // active selection needs a reference to the canvas.
//                        clonedObj.canvas = canvas;
//                        clonedObj.forEachObject(function(obj) {
//                            canvas.add(obj);
//                        });
//                        // this should solve the unselectability
//                        clonedObj.setCoords();
//                    } else {
//                        canvas.add(clonedObj);
//                    }
//                    cloned.top += 10;
//                    cloned.left += 10;
//                    canvas.setActiveObject(clonedObj);
//                    canvas.requestRenderAll();
//                });
//            });
//        } else if (e.keyCode == 27) { //Esc:閉じる
//            window.close();
//        } else if (e.keyCode == 32) { //Space:フリーズ
//            if (background_type == "camera" || background_type == "movie") freeze(false);
//        } else if (e.keyCode == 37 && is_focus_zoomrange == false) { //←:アンドゥ
//            undo();
//        } else if (e.keyCode == 38 && is_focus_zoomrange == false) { //↑:選択中アイテム手前移動
//            //canvas.bringToFront(canvas.getActiveObject()); //一番手前
//            canvas.bringForward(canvas.getActiveObject()); //一つ手前
//            canvas.renderAll(); //更新
//        } else if (e.keyCode == 39 && is_focus_zoomrange == false) { //→:リドゥ
//            redo();
//        } else if (e.keyCode == 40 && is_focus_zoomrange == false) { //↓:選択中アイテム奥移動
//            //canvas.sendToBack(canvas.getActiveObject()); //一番奥
//            canvas.sendBackwards(canvas.getActiveObject()); //一つ奥
//            canvas.renderAll(); //更新
//        } else if (e.keyCode == 8 || e.keyCode == 46) { //BackSpace/Delete:選択中アイテム削除
//            remove_object(canvas.getActiveObject());
//        }
//    }
//}, false);
//addEventListener("touchstart", function(e) {
//    touchstart_bar = 0;
//    touchmove_bar = 0;
//    if (e.touches.length > 1) {
//        //ピンチ開始時に描画されたくないので変更
//        canvas.isDrawingMode = false;
//        //絶対値を取得
//        w_abs_start = Math.abs(e.touches[1].pageX - e.touches[0].pageX);
//        h_abs_start = Math.abs(e.touches[1].pageY - e.touches[0].pageY);
//        //はじめに2本指タッチした時の面積
//        touchstart_bar = w_abs_start*h_abs_start;
//    }
//}, false);
//addEventListener("touchmove", function(e) {
//    if (e.touches.length > 1) {
//        //絶対値を取得
//        w_abs_move = Math.abs(e.touches[1].pageX - e.touches[0].pageX);
//        h_abs_move = Math.abs(e.touches[1].pageY - e.touches[0].pageY);
//        //ムーブした時の面積
//        touchmove_bar = w_abs_move*h_abs_move;
//        //はじめに2タッチ面積からムーブした時の面積を引く
//        area_bar = touchstart_bar-touchmove_bar;
//        if (area_bar　<　0) {//拡大する
//            scale_zoom = parseFloat(scale_zoom) + parseFloat(0.2);
//        }
//        else if (area_bar　>　0) {//縮小する
//            scale_zoom = parseFloat(scale_zoom) - parseFloat(0.2);
//        }
//        if (scale_zoom < 1.0) {
//            scale_zoom = 1.0;
//        } else if (scale_zoom > 8.0) {
//            scale_zoom = 8.0;
//        }
//        $("input[type=range]").val(scale_zoom).change();
//    }
//}, false);

//remove object
function remove_object(obj) {
    if (item_mask == obj) return; //マスクのオブジェクトは消さない
    if (item_highlight == obj || item_highlight_bg_t == obj || item_highlight_bg_b == obj || item_highlight_bg_l == obj || item_highlight_bg_r == obj) return; //ハイライトのオブジェクトは消さない
    if (list_item_compare.includes(obj)) return; //コンペアのオブジェクトは消さない
    listUndo.push(obj);
    canvas.remove(obj);
    canvas.renderAll(); //更新
}

//Drag & Drop
var list_image_compare = [];
var list_item_compare = [];
var list_path_compare = [];
var list_selecteditem_compare = [];
//$(function() {
//    if (!window.FileReader) {
//        alert("not supported File API.");
//        return false;
//    }
//    var droparea = $("#draw");
//    var cancelEvent = function(e) {
//        e.preventDefault();
//        e.stopPropagation();
//        return false;
//    }
//    droparea.bind("dragleave", function(e) { });
//    droparea.bind("dragenter", cancelEvent);
//    droparea.bind("dragover", cancelEvent);
//    droparea.bind("drop", function(e) {
//        cancelEvent(e);
//        is_compare = true; //Drag & Dropでも一応コンペアしたことにする
//        var length = list_image_compare.length + e.originalEvent.dataTransfer.files.length;
//        for (var i = 0; i < e.originalEvent.dataTransfer.files.length; i++) {
//            var file = e.originalEvent.dataTransfer.files[i];
//            if (list_path_compare.indexOf(file.name) < 0) {
//                list_path_compare.push(file.name);
//            } else {
//                length--;
//                continue;
//            }
//            console.log(file.name);
//            loadImageItem(length - e.originalEvent.dataTransfer.files.length + i, length, file);
//        }
//        setTimeout(function() {
//            var list = [];
//            for (var i = 0; i < list_path_compare.length; i++) {
//                for (var j = 0; j < list_image_compare.length; j++) {
//                    var img = list_image_compare[j];
//                    if (list_path_compare[i].includes(img.cacheKey)) {
//                        list.splice(i, 0, img);
//                    }
//                }
//            }
//            list_image_compare = list;
//            relocateImageItem(length); //既に配置されている分を再配置
//        }, e.originalEvent.dataTransfer.files.length * 100);
//        return false;
//    });
//});
//function loadImageItem(i, length, file) {
//    var reader = new FileReader();
//    reader.onload = function(e) {
//        fabric.Image.fromURL(reader.result, function(img) {
//            if (i < 9) {
//                img.set({ cacheKey: file.name }); //cacheKeyを使っていないのでファイル名を入れておく
//                list_image_compare.push(img);
//            }
//        });
//    }
//    reader.readAsDataURL(file);
//}
function relocateImageItem(length) {
    var list_checked = [];
    for (var i = 0; i < list_item_compare.length; i++) {
        if (list_item_compare[i].getObjects().length > 1) {
            list_selecteditem_compare.splice(list_selecteditem_compare.indexOf(list_item_compare[i]), 1);
            list_checked.push(i);
        }
        canvas.remove(list_item_compare[i]);
    }
    list_item_compare = []; //登録アイテムリストクリア
    canvas.backgroundColor = "rgba(0, 0, 0, 1.0)"; //背景黒
    for (var i = 0; i < list_image_compare.length; i++) {
        addImageItem(i, length, list_image_compare[i], true, list_checked.includes(i)); //画像をcanvasに追加
    }
}
var canvas_w = 0, canvas_h = 0, margin_compare = 5;
function addImageItem(index, length, img, is_reglist, is_check) {
    if (length == 1) { //配置する枚数
        canvas_w = Math.ceil((canvas.width - margin_compare * 4) / 1); canvas_h = Math.ceil(canvas.height / 1);
    } else if (length == 2) {
        canvas_w = Math.ceil((canvas.width - margin_compare * 4) / 2); canvas_h = Math.ceil(canvas.height / 1);
    } else if (3 <= length && length <= 4) {
        canvas_w = Math.ceil((canvas.width - margin_compare * 4) / 2); canvas_h = Math.ceil((canvas.height - margin_compare * 4) / 2);
    } else if (5 <= length && length <= 6) {
        canvas_w = Math.ceil((canvas.width - margin_compare * 6) / 3); canvas_h = Math.ceil((canvas.height - margin_compare * 4) / 2);
    } else {
        canvas_w = Math.ceil((canvas.width - margin_compare * 6) / 3); canvas_h = Math.ceil((canvas.height - margin_compare * 6) / 3);
    }
    var canvasAspect = canvas_w / canvas_h;
    var imgAspect = img.width / img.height;
    var left, top, scaleFactor;
    if (canvasAspect <= imgAspect) { //倍率
        scaleFactor = canvas_w / img.width;
    } else {
        scaleFactor = canvas_h / img.height;
    }
    if (index == 0) { //1枚目
        if (canvasAspect <= imgAspect) {
            left = margin_compare; top = -((img.height * scaleFactor) - canvas_h) / 2 + margin_compare;
        } else {
            left = -((img.width * scaleFactor) - canvas_w) / 2 + margin_compare; top = margin_compare;
        }
    } else if (index == 1) { //2枚目
        if (canvasAspect <= imgAspect) {
            left = canvas_w + margin_compare * 3; top = -((img.height * scaleFactor) - canvas_h) / 2 + margin_compare;
        } else {
            left = -((img.width * scaleFactor) - canvas_w) / 2 + canvas_w + margin_compare * 3; top = margin_compare;
        }
    } else if (index == 2) { //3枚目
        if (3 <= length && length <= 4) {
            if (canvasAspect <= imgAspect) {
                left = margin_compare; top = -((img.height * scaleFactor) - canvas_h) / 2 + canvas_h + margin_compare * 3;
            } else {
                left = -((img.width * scaleFactor) - canvas_w) / 2 + margin_compare; top = canvas_h + margin_compare * 3;
            }
        } else {
            if (canvasAspect <= imgAspect) {
                left = canvas_w * 2 + margin_compare * 5; top = -((img.height * scaleFactor) - canvas_h) / 2 + margin_compare;
            } else {
                left = -((img.width * scaleFactor) - canvas_w) / 2 + canvas_w * 2 + margin_compare * 5; top = margin_compare;
            }
        }
    } else if (index == 3) { //4枚目
        if (3 <= length && length <= 4) {
            if (canvasAspect <= imgAspect) {
                left = canvas_w + margin_compare * 3; top = -((img.height * scaleFactor) - canvas_h) / 2 + canvas_h + margin_compare * 3;
            } else {
                left = -((img.width * scaleFactor) - canvas_w) / 2 + canvas_w + margin_compare * 3; top = canvas_h + margin_compare * 3;
            }
        } else {
            if (canvasAspect <= imgAspect) {
                left = margin_compare; top = -((img.height * scaleFactor) - canvas_h) / 2 + canvas_h + margin_compare * 3;
            } else {
                left = -((img.width * scaleFactor) - canvas_w) / 2 + margin_compare; top = canvas_h + margin_compare * 3;
            }
        }
    } else if (index == 4) { //5枚目
        if (canvasAspect <= imgAspect) {
            left = canvas_w + margin_compare * 3; top = -((img.height * scaleFactor) - canvas_h) / 2 + canvas_h + margin_compare * 3;
        } else {
            left = -((img.width * scaleFactor) - canvas_w) / 2 + canvas_w + margin_compare * 3; top = canvas_h + margin_compare * 3;
        }
    } else if (index == 5) { //6枚目
        if (canvasAspect <= imgAspect) {
            left = canvas_w * 2 + margin_compare * 5; top = -((img.height * scaleFactor) - canvas_h) / 2 + canvas_h + margin_compare * 3;
        } else {
            left = -((img.width * scaleFactor) - canvas_w) / 2 + canvas_w * 2 + margin_compare * 5; top = canvas_h + margin_compare * 3;
        }
    } else if (index == 6) { //7枚目
        if (canvasAspect <= imgAspect) {
            left = margin_compare; top = -((img.height * scaleFactor) - canvas_h) / 2 + canvas_h * 2 + margin_compare * 5;
        } else {
            left = -((img.width * scaleFactor) - canvas_w) / 2 + margin_compare; top = canvas_h * 2 + margin_compare * 5;
        }
    } else if (index == 7) { //8枚目
        if (canvasAspect <= imgAspect) {
            left = canvas_w + margin_compare * 3; top = -((img.height * scaleFactor) - canvas_h) / 2 + canvas_h * 2 + margin_compare * 5;
        } else {
            left = -((img.width * scaleFactor) - canvas_w) / 2 + canvas_w + margin_compare * 3; top = canvas_h * 2 + margin_compare * 5;
        }
    } else if (index == 8) { //9枚目
        if (canvasAspect <= imgAspect) {
            left = canvas_w * 2 + margin_compare * 5; top = -((img.height * scaleFactor) - canvas_h) / 2 + canvas_h * 2 + margin_compare * 5;
        } else {
            left = -((img.width * scaleFactor) - canvas_w) / 2 + canvas_w * 2 + margin_compare * 5; top = canvas_h * 2 + margin_compare * 5;
        }
    } else { //上限オーバー
        return;
    }
    var item = img.set({
       top: top,
       left: left,
       originX: 'left',
       originY: 'top',
       scaleX: scaleFactor,
       scaleY: scaleFactor,
       stroke: "black", strokeWidth : margin_compare,
    });
    var check = fabric.util.object.clone(item_check);
    check.set({top: top, left: left});
    canvas.discardActiveObject();
    var group = null;
    if (is_check == true) {
        group = new fabric.Group([ item, check ], { top: top, left: left, angle: 0 });
        list_selecteditem_compare.splice(list_selecteditem_compare.length, 0, group);
    } else {
        group = new fabric.Group([ item ], { top: top, left: left, angle: 0 });
    }
    group.hasControls = group.hasBorders = false;
    canvas.add(group);
    canvas.sendToBack(group); //一番奥
    if (is_reglist == true) {
        list_item_compare.splice(index, 0, group);
    } else {
        list_item_compare.splice(index, 1, group);
    }
    canvas.discardActiveObject();
    canvas.renderAll(); //更新
}
var item_check;
fabric.Image.fromURL("images/Check.png", function(img) {
    item_check = img.set({
        top: 0,
        left: 0,
    });
    item_check.scaleToWidth(100);
    item_check.scaleToHeight(100);
});
