///////////////////////////////////////////////////////////////////////////////////////////////////
//宣言
///////////////////////////////////////////////////////////////////////////////////////////////////
var USE_LOCAL = true; //EITやデバッグなどローカルで使用する場合は有効にする

//定義
var MODE = {
    CAMERA : "Camera",
    WHITEBOARD1 : "Whiteboard1",
    WHITEBOARD2 : "Whiteboard2",
    WHITEBOARD3 : "Whiteboard3",
    WHITEBOARD4 : "Whiteboard4",
    TRANSPARENT : "Transparent",
    CONTENTS : "Contents",
    IMAGE : "image",
    VIDEO : "video",
    EDUCATIONAL_MATERIAL : "teaching_material",
};

//ステータス
var timeout_status = 2000; //消えるまでの時間

//canvasの読み込み設定(線描画)
var can = $("#canvas").get(0);
var ctx = can.getContext("2d");

//描画
var opacity_marker = 0.5; //自由線の透過度
var width_pen = 2 * 5; //自由線の太さ
var width_marker = 2 * 5; //自由線の太さ
var color_pen = "rgba(0,0,0,1.0)"; //自由線の色
var color_marker = "rgba(255,255,0,1.0)"; //自由線の色
var color_text = "rgb(0,0,0)"; //文字の色
var color_text_background = "rgb(239,228,179)"; //文字背景の色
var font_size_text = 30;
var font_family_text = "Arial";
var font_weight_text = "";
var imgInsertPath = null; //画像挿入のファイルパス
var background_type = "whiteboard1"; //初期背景は白板
var id_timeout_setitemtype = null;

//カメラ背景
var imgLiveTmp = null; //状態遷移時に使用するための画像データ
var deviceid = null; //デバイスID
var camera_w = 1280;//解像度幅
var camera_h = 720; //解像度高さ
var camera_fr = 30; //フレームレート
var start_retry = 0;

//ホワイトボード背景
var imgDefaultTmp1 = null; //
var imgDefaultTmp2 = null; //
var imgDefaultTmp3 = null; //
var imgDefaultTmp4 = null; //

//コンテンツ背景
var selectContents =　document.createElement('input');
selectContents.type　=　"file";
selectContents.id　=　'selectfile';
selectContents.name = 'selectfile';
selectContents.style = 'display:none';
selectContents.accept = '.png, .jpg, .bmp, .mp4, .webm, .pdf';

//静止画背景
var imgBackgroundTmp = null; //状態遷移時に使用するための画像データ

//動画背景
var videoBackgroundTmp = null;

//教材背景
var tmBackgroundTmp = null;

//録画
var is_recording = false;

//画像挿入
var selectInsertImage =　document.createElement('input');
selectInsertImage.type　=　"file";
selectInsertImage.id　=　'selectfileinsert';
selectInsertImage.name = 'selectfileinsert';
selectInsertImage.style = 'display:none';
selectInsertImage.accept = '.png, .jpg, .bmp';

//アップロード
//var selectUpload =　document.createElement('input');
//selectUpload.type　=　"file";
//selectUpload.id　=　'selectfileupload';
//selectUpload.name = 'selectfileupload';
//selectUpload.style = 'display:none';
//selectUpload.accept = '.png, .jpg, .bmp, .mp4, .webm';

//初期表示
$("#live").css('background-color', 'transparent'); //video透明背景
document.getElementById("video").style.display = "none"; //video非表示
$(".pencil").attr('src','images/PEN_Black_S.png');
//$("#pen_width_1").attr('src','images/SET_line1_P.png');
//$("#pen_width_2").attr('src','images/SET_line2_S.png');
//$("#pen_width_3").attr('src','images/SET_line3_P.png');
//$("#pen_width_4").attr('src','images/SET_line4_P.png');
//$("#pen_width_5").attr('src','images/SET_line5_P.png');
$("#pen_black").attr('src','images/SET_color_BLACK_S.png');
$("#pen_red").attr('src','images/SET_color_RED_P.png');
$("#pen_green").attr('src','images/SET_color_GREEN_P.png');
$("#pen_blue").attr('src','images/SET_color_BLUE_P.png');
$("#pen_yellow").attr('src','images/SET_color_YELLOW_P.png');
$("#pen_magenta").attr('src','images/SET_color_MAGENTA_P.png');
$("#pen_cyan").attr('src','images/SET_color_CYAN_P.png');
$("#pen_white").attr('src','images/SET_color_WHITE_P.png');
$(".marker").attr('src','images/MARKER_Yellow_N.png');
//$("#marker_opacity_8").attr('src','images/SET_PEN_P.png');
//$("#marker_opacity_5").attr('src','images/SET_MARKER_S.png');
//$("#marker_width_1").attr('src','images/SET_line1_P.png');
//$("#marker_width_2").attr('src','images/SET_line2_S.png');
//$("#marker_width_3").attr('src','images/SET_line3_P.png');
//$("#marker_width_4").attr('src','images/SET_line4_P.png');
//$("#marker_width_5").attr('src','images/SET_line5_P.png');
$("#marker_black").attr('src','images/SET_color_BLACK_P.png');
$("#marker_red").attr('src','images/SET_color_RED_P.png');
$("#marker_green").attr('src','images/SET_color_GREEN_P.png');
$("#marker_blue").attr('src','images/SET_color_BLUE_P.png');
$("#marker_yellow").attr('src','images/SET_color_YELLOW_S.png');
$("#marker_magenta").attr('src','images/SET_color_MAGENTA_P.png');
$("#marker_cyan").attr('src','images/SET_color_CYAN_P.png');
$("#marker_white").attr('src','images/SET_color_WHITE_P.png');
$(".clearObject").attr('src','images/Eraser_part_N.png');
$(".selectitem").attr('src','images/Hand_N.png');
$('#modeList').selectpicker();
//$('.selectitem').fadeOut("fast");
$('.recording').fadeOut("fast");
$(".rotation").fadeOut("fast");
$(".freeze").fadeOut("fast");
$(".statuscap").fadeOut("fast");
$(".statusrec").fadeOut("fast");
$(".statusfreeze").fadeOut("fast");

///////////////////////////////////////////////////////////////////////////////////////////////////
//言語判定
///////////////////////////////////////////////////////////////////////////////////////////////////
var TEXT_BROWSEER_UNSUPPORTED = " is an unsupported browser. Exit the application.";
var TEXT_RECORDING_END = "Because the background has been changed, stop recording.";
var TEXT_CAMERALIST = "Camera selection";
var TEXT_MODE_CAMERA = "Camera";
var TEXT_CANNOT_USE_CAMERA = "This camera can not be used now.";
var TEXT_CONTENTS_IMAGE = "Image";
var TEXT_CONTENTS_VIDEO = "Video";
var TEXT_CONTENTS_EDUCATIONAL_MATERIAL = "Educational material";
var TEXT_COOKIE_CONSENT = "This site uses cookies for notification of access date and time etc.\nDo you agree to use cookies?";
var TEXT_INPUT_EDUCATIONAL_MATERIAL_PATH = "Please enter the path or URL of the educational material file.";
var TEXT_TOOLTIP_OPEN = "Display menu";
var TEXT_TOOLTIP_CLOSE = "Hide the menu";
var TEXT_TOOLTIP_PEN = "Pen";
var TEXT_TOOLTIP_MARKER = "Marker";
var TEXT_TOOLTIP_ERASER = "Eraser";
var TEXT_TOOLTIP_SELECT = "Selection";
var TEXT_TOOLTIP_CAP = "Capture";
var TEXT_TOOLTIP_REC = "Recording";
var TEXT_TOOLTIP_ROTATION = "Rotate background";
var TEXT_TOOLTIP_FREEZE = "Freeze";
var TEXT_TOOLTIP_ZOOM = "Scale";
var TEXT_TOOLTIP_FULLSCREEN = "Full screen";
var TEXT_TOOLTIP_HELP = "Help";
var TEXT_TOOLTIP_MENUPOS = "Toggle menu position";
//ブラウザの言語設定の中で先頭の言語を取得
var language = (window.navigator.languages && window.navigator.languages[0]) ||
            window.navigator.language ||
            window.navigator.userLanguage ||
            window.navigator.browserLanguage;
if (language.includes("ja")) {
    TEXT_BROWSEER_UNSUPPORTED = "はサポートされていないブラウザです。アプリケーションを終了します。";
    TEXT_RECORDING_END = "背景が変更されたため、録画を停止します。";
    TEXT_CAMERALIST = "カメラ選択";
    TEXT_MODE_CAMERA = "カメラ";
    TEXT_CANNOT_USE_CAMERA = "このカメラは現在使用できません。";
    TEXT_CONTENTS_IMAGE = "画像";
    TEXT_CONTENTS_VIDEO = "動画";
    TEXT_CONTENTS_EDUCATIONAL_MATERIAL = "Web教材";
    TEXT_COOKIE_CONSENT = "このサイトではアクセス日時などの通知にcookieを使用しています。\ncookieを使用することに同意しますか?";
    TEXT_INPUT_EDUCATIONAL_MATERIAL_PATH = "Web教材のURLを入力してください。";
    TEXT_TOOLTIP_OPEN = "メニュー表示";
    TEXT_TOOLTIP_CLOSE = "メニュー非表示";
    TEXT_TOOLTIP_PEN = "ペン";
    TEXT_TOOLTIP_MARKER = "マーカー";
    TEXT_TOOLTIP_ERASER = "消しゴム";
    TEXT_TOOLTIP_SELECT = "選択";
    TEXT_TOOLTIP_CAP = "キャプチャー";
    TEXT_TOOLTIP_REC = "録画";
    TEXT_TOOLTIP_ROTATION = "背景を回転";
    TEXT_TOOLTIP_FREEZE = "フリーズ";
    TEXT_TOOLTIP_ZOOM = "拡大";
    TEXT_TOOLTIP_FULLSCREEN = "全画面";
    TEXT_TOOLTIP_HELP = "ヘルプ";
    TEXT_TOOLTIP_MENUPOS = "メニュー位置切替";
} else if (language.includes("fr")) {
    TEXT_BROWSEER_UNSUPPORTED = " est un navigateur non pris en charge. Quittez l'application.";
    TEXT_RECORDING_END = "Parce que l'arriere-plan a ete change, arretez l'enregistrement.";
    TEXT_CAMERALIST = "selection de camera";
    TEXT_MODE_CAMERA = "camera";
    TEXT_CANNOT_USE_CAMERA = "Cette camera ne peut pas etre utilisee maintenant.";
    TEXT_CONTENTS_IMAGE = "Image";
    TEXT_CONTENTS_VIDEO = "Video";
    TEXT_COOKIE_CONSENT = "Ce site utilise des cookies pour la notification de la date et l'heure d'accès, etc.\nAcceptez-vous d'utiliser des cookies?";
} else if (language.includes("de")) {
    TEXT_BROWSEER_UNSUPPORTED = " ist ein nicht unterstutzter Browser. Beenden Sie die Anwendung.";
    TEXT_RECORDING_END = "Da der Hintergrund geandert wurde, beenden Sie die Aufnahme.";
    TEXT_CAMERALIST = "Kameraauswahl";
    TEXT_MODE_CAMERA = "Kamera";
    TEXT_CANNOT_USE_CAMERA = "Diese Kamera kann jetzt nicht verwendet werden.";
    TEXT_CONTENTS_IMAGE = "Bild";
    TEXT_CONTENTS_VIDEO = "Video";
    TEXT_COOKIE_CONSENT = "Diese Seite verwendet Cookies für die Benachrichtigung über Zugriffsdatum und -zeit usw.\nStimmen Sie der Verwendung von Cookies zu?";
}
if (language.includes("ja")) {
    $('#modeList').append('<option id="wbmode1" value="Whiteboard1" data-content="<img src=\'./images/WHITEBOARD.png\' style=\'width:46px; height:46px;\'>&nbsp;<span style=\'display:inline-block;\'>ホワイトボード 1</span>"></option>'); //要素を追加
    $('#modeList').append('<option id="wbmode2" value="Whiteboard2" data-content="<img src=\'./images/WHITEBOARD.png\' style=\'width:46px; height:46px;\'>&nbsp;<span style=\'display:inline-block;\'>ホワイトボード 2</span>"></option>'); //要素を追加
    $('#modeList').append('<option id="wbmode3" value="Whiteboard3" data-content="<img src=\'./images/WHITEBOARD.png\' style=\'width:46px; height:46px;\'>&nbsp;<span style=\'display:inline-block;\'>ホワイトボード 3</span>"></option>'); //要素を追加
    $('#modeList').append('<option id="wbmode4" value="Whiteboard4" data-content="<img src=\'./images/WHITEBOARD.png\' style=\'width:46px; height:46px;\'>&nbsp;<span style=\'display:inline-block;\'>ホワイトボード 4</span>"></option>'); //要素を追加
    if (USE_LOCAL == true) $('#modeList').append('<option id="transparent" value="Transparent" data-content="<img src=\'./images/WHITEBOARD.png\' style=\'width:46px; height:46px;\'>&nbsp;<span style=\'display:inline-block;\'>透明</span>"></option>'); //要素を追加
    $('#modeList').append('<option id="contentsmode" value="Contents" data-content="<img src=\'./images/CONTENTS.png\' style=\'width:46px; height:46px;\'>&nbsp;<span style=\'display:inline-block;\'>コンテンツ</span>"></option>'); //要素を追加
    $('#modeList').append('<option id="teaching_material" value="teaching_material" data-content="<img src=\'./images/EducationalMaterial.png\' style=\'width:40px; height:40px;\'>&nbsp;<span style=\'display:inline-block;\'>' + TEXT_CONTENTS_EDUCATIONAL_MATERIAL + '</span>"></option>'); //要素を追加
    $('#modeList').selectpicker("refresh"); //リスト更新
} else {
    $('#modeList').append('<option id="wbmode1" value="Whiteboard1" data-content="<img src=\'./images/WHITEBOARD.png\' style=\'width:46px; height:46px;\'>&nbsp;<span style=\'display:inline-block;\'>Whiteboard 1</span>"></option>'); //要素を追加
    $('#modeList').append('<option id="wbmode2" value="Whiteboard2" data-content="<img src=\'./images/WHITEBOARD.png\' style=\'width:46px; height:46px;\'>&nbsp;<span style=\'display:inline-block;\'>Whiteboard 2</span>"></option>'); //要素を追加
    $('#modeList').append('<option id="wbmode3" value="Whiteboard3" data-content="<img src=\'./images/WHITEBOARD.png\' style=\'width:46px; height:46px;\'>&nbsp;<span style=\'display:inline-block;\'>Whiteboard 3</span>"></option>'); //要素を追加
    $('#modeList').append('<option id="wbmode4" value="Whiteboard4" data-content="<img src=\'./images/WHITEBOARD.png\' style=\'width:46px; height:46px;\'>&nbsp;<span style=\'display:inline-block;\'>Whiteboard 4</span>"></option>'); //要素を追加
    if (USE_LOCAL == true) $('#modeList').append('<option id="transparent" value="Transparent" data-content="<img src=\'./images/WHITEBOARD.png\' style=\'width:46px; height:46px;\'>&nbsp;<span style=\'display:inline-block;\'>Transparent</span>"></option>'); //要素を追加
    $('#modeList').append('<option id="contentsmode" value="Contents" data-content="<img src=\'./images/CONTENTS.png\' style=\'width:46px; height:46px;\'>&nbsp;<span style=\'display:inline-block;\'>Contents</span>"></option>'); //要素を追加
    $('#modeList').append('<option id="teaching_material" value="teaching_material" data-content="<img src=\'./images/EducationalMaterial.png\' style=\'width:40px; height:40px;\'>&nbsp;<span style=\'display:inline-block;\'>' + TEXT_CONTENTS_EDUCATIONAL_MATERIAL + '</span>"></option>'); //要素を追加
    $('#modeList').selectpicker("refresh"); //リスト更新
}
if (language.includes("ja") || language.includes("en")) { //欧州以外の言語についてはメッセージ表示しないのでこちらに追加していく
    /* 言語が日本語か英語の場合はcookie使用許可のメッセージは表示しない */
} else {
//    showCookieConsentMessage(); //基本的には念のためcookieの使用許可メッセージを表示する
}
function showCookieConsentMessage() {
    if(!confirm(TEXT_COOKIE_CONSENT)){
        /* キャンセルの時の処理 */
        window.open('about:blank','_self').close();
    }else{
        /*　OKの時の処理 */
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//ブラウザ判定
///////////////////////////////////////////////////////////////////////////////////////////////////
var userAgent = window.navigator.userAgent.toLowerCase(); //判定順番が重要なので順番は入れ替えない
var useBrowser = "chrome";
if(userAgent.indexOf('msie') != -1) { //IE 6 - 10
    alert("IE" + TEXT_BROWSEER_UNSUPPORTED ); window.open('about:blank','_self').close();
    useBrowser = "ie10";
} else if(userAgent.indexOf('trident') != -1) { //IE 11
    alert("IE" + TEXT_BROWSEER_UNSUPPORTED); window.open('about:blank','_self').close();
    useBrowser = "ie11";
} else if(userAgent.indexOf('edge') != -1) { //Edge
    //alert("Edge" + TEXT_BROWSEER_UNSUPPORTED); window.open('about:blank','_self').close();
    useBrowser = "edge";
} else if(userAgent.indexOf('chrome') != -1) { //Chrome
    //alert("Chrome" + TEXT_BROWSEER_UNSUPPORTED); window.open('about:blank','_self').close();
    useBrowser = "chrome";
} else if(userAgent.indexOf('safari') != -1) { //Safari
    //alert("Safari " + TEXT_BROWSEER_UNSUPPORTED); window.open('about:blank','_self').close();
    var ver = userAgent.slice(userAgent.indexOf("version/") + 8, userAgent.indexOf("version/") +  8 + 2);
    if (ver.indexOf(".") != -1 || parseInt(ver) == 10) {
        alert("WebRTC's support is from Safari version 11.");
        useBrowser = "safari_not_support_webrtc";
    } else {
        useBrowser = "safari";
    }
} else if(userAgent.indexOf('firefox') != -1) { //Firefox
    alert("Firefox" + TEXT_BROWSEER_UNSUPPORTED); window.open('about:blank','_self').close();
    useBrowser = "firefox";
} else if(userAgent.indexOf('opera') != -1) { //Opera
    alert("Opera" + TEXT_BROWSEER_UNSUPPORTED); window.open('about:blank','_self').close();
    useBrowser = "opera";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//tooltip
///////////////////////////////////////////////////////////////////////////////////////////////////
$(".menuup").attr('title',TEXT_TOOLTIP_OPEN);
$(".menudown").attr('title',TEXT_TOOLTIP_CLOSE);
$(".pencil").attr('title',TEXT_TOOLTIP_PEN);
$(".marker").attr('title',TEXT_TOOLTIP_MARKER);
$(".clearObject").attr('title',TEXT_TOOLTIP_ERASER);
$(".selectitem").attr('title',TEXT_TOOLTIP_SELECT);
$(".captureall").attr('title',TEXT_TOOLTIP_CAP);
$(".recording").attr('title',TEXT_TOOLTIP_REC);
$(".rotation").attr('title',TEXT_TOOLTIP_ROTATION);
$(".freeze").attr('title',TEXT_TOOLTIP_FREEZE);
$(".zoomrange").attr('title',TEXT_TOOLTIP_ZOOM);
$(".fullscreen").attr('title',TEXT_TOOLTIP_FULLSCREEN);
$(".help").attr('title',TEXT_TOOLTIP_HELP);
$(".positionchange").attr('title',TEXT_TOOLTIP_MENUPOS);

///////////////////////////////////////////////////////////////////////////////////////////////////
//画面リサイズ
///////////////////////////////////////////////////////////////////////////////////////////////////
var scale_window = 1.000;
var id_timeout_resize = null;
var is_show_setting_tmp = false;
function canvas_resize() {
    if (id_timeout_resize != null) {
        clearTimeout(id_timeout_resize);
    }
//    $(".iframe").colorbox.close();
    scale_window = parseFloat(window.innerWidth) / 1680.0; //1920x1200のディスプレイを元に計算する
    id_timeout_resize = setTimeout(canvas_resize_timeout, 300);
    if (is_show_setting == true) {
        is_show_setting_tmp = is_show_setting;
        hideSetting();
    }
    if (is_mask == true) {
        changeModeMask(0, 0, 0, 0, "rgba(0 ,0, 0, 0)");
    }
    if (is_highlight == true) {
        changeModeHighlight(0, 0, 0, 0, "rgba(0 ,0, 0, 0)");
    }
}
function canvas_resize_timeout() {
    $("#menu").animate({ 'zoom': scale_window }, { complete: canvas_resize_finished });
}
function canvas_resize_finished() {
    //画面のサイズ取得
    var windowInnerWidth = $("#draw").width();
    var windowInnerHeight = $("#draw").height();

    //教材タグサイズ
    $("#tmview").css('height', $("#draw").height());

    //canvasリサイズ
    canvas.setWidth(windowInnerWidth);
    canvas.setHeight(windowInnerHeight);

    //カメラライブリサイズ
    var theVideo = document.getElementById('video');
    theVideo.setAttribute('width',windowInnerWidth);
    theVideo.setAttribute('height',windowInnerHeight);

    //背景再描画
//    if (background_type.includes("whiteboard")) {
//        if (background_type == "whiteboard1") {
//            drawBackground(imgDefaultTmp1);
//        } else if (background_type == "whiteboard2") {
//            drawBackground(imgDefaultTmp2);
//        } else if (background_type == "whiteboard3") {
//            drawBackground(imgDefaultTmp3);
//        } else if (background_type == "whiteboard4") {
//            drawBackground(imgDefaultTmp4);
//        }
//    } else if (background_type == "image") {
//        if (imgBackgroundTmp != null) {
//            drawBackground(imgBackgroundTmp);
//        }
//    }

    if (is_compare == true) {
        relocateImageItem(list_image_compare.length);
    }

    //メニュー表示
//    if (is_show_setting_tmp == true || is_init == true) { //リサイズ時と初回はメニューを表示させる
//        if (is_init == true && localStorage.getItem("EITC_nextDisplay") == "false") $('.iframe')[0].click(); //初回はヘルプ表示
//        $(".moveup").slideUp("fast", function() {
//            is_show_setting_tmp = false; is_init = false; showSetting();
//        });
//    }
}
window.addEventListener('resize', canvas_resize, false);
canvas_resize();


///////////////////////////////////////////////////////////////////////////////////////////////////
//カウンタ
///////////////////////////////////////////////////////////////////////////////////////////////////
var COOKIE_KEY_VISIT = "visit_date_pass";
var pass_data_visit; //前回訪問した時間
//現在の時間を取得する関数
function getNowTime(){
  nowDate = new Date();
  nowTime = nowDate.getTime();  //現在の日時取得
}
//変数nからの経過時間を求める関数
function courseDate(n){
  getNowTime();
  course = nowTime - n;
  courseTime = Math.floor(course/(1000*60*60*24));  //日数に換算
}
//cookieを削除
function remove_cookie(){
//  $.removeCookie(COOKIE_KEY_VISIT);
//  console.log("remove cookie.");
}
//実行処理
$(function(){
//    //cookieの読み込み
//    cookieArr = $.cookie();

//    //前回訪問した時からの経過時間の処理
//    var total = 0; //合計
//    var now = new Date();
//    if (cookieArr[COOKIE_KEY_VISIT] == null) {  //初訪問の時
//        firebase.database().ref('counts/total').once('value').then(function(snapshot) {
//            if (snapshot.val() > 0) { //初訪問だが他ユーザーのアクセスが既にある場合
//                total = snapshot.val() + 1;
//            } else { //初訪問でdatabaseも空の場合
//                console.log("The key was not found. >> total");
//                remove_cookie();
//                total = 1;
//            }
//            firebase.database().ref('counts/total').set(total);
//            firebase.database().ref('counts/dates/' + total).set(now.toString());
//            console.log("This is the first visit. >> total[" + total + "]");
//        });
//        reg_cookie_first();
//    } else {  //2回目以降の訪問の時
//        pass_data_visit = $.cookie(COOKIE_KEY_VISIT);
//        courseDate(pass_data_visit);
//        $.cookie(COOKIE_KEY_VISIT, nowTime);
//        if (courseTime > 0) { //前回からの日を跨いでいる場合
//            firebase.database().ref('counts/total').once('value').then(function(snapshot) {
//                if (snapshot.val() > 0) { //通常処理
//                    total = snapshot.val() + 1;
//                    firebase.database().ref('counts/total').set(total);
//                    firebase.database().ref('counts/dates/' + total).set(now.toString());
//                    console.log(courseTime + " day passed since the last visit. >> total[" + total + "]");
//                } else { //databaseの値が無い場合はcookie消して最初の登録だけする
//                    reg_cookie_keynotfound(now, total);
//                }
//            });
//        } else if (Number.isNaN(courseTime) == false) { //前回からの日を跨いでいない場合
//            firebase.database().ref('counts/total').once('value').then(function(snapshot) {
//                if (snapshot.val() <= 0) { //cookieに登録した日時から日付は変わっていないがdatabaseの値が無い場合はcookie消して最初の登録だけする
//                    reg_cookie_keynotfound(now, total);
//                }
//            });
//        } else { //err
//            console.log("Database check failed.");
//            remove_cookie();
//            firebase.database().ref('counts/total').set(0);
//        }
//    }
});
function reg_cookie_first() {
//    getNowTime();
//    pass_data_visit = nowTime;
//    courseDate(pass_data_visit);
//    $.cookie(COOKIE_KEY_VISIT, pass_data_visit);
}
function reg_cookie_keynotfound(now, total) {
//    console.log("The key was not found. >> total");
//    remove_cookie();
//    total = 1;
//    firebase.database().ref('counts/total').set(total);
//    firebase.database().ref('counts/dates/' + total).set(now.toString());
//    reg_cookie_first();
//    console.log(courseTime + " day passed since the last visit. >> total[" + total + "]");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//メニュー表示
///////////////////////////////////////////////////////////////////////////////////////////////////
var is_init = true; //初期は表示
var is_show_setting = false;
var is_show_setting_pen = false;
var is_show_setting_marker = false;
var is_show_setting_eraser = false;
var is_busy = false;
function showSetting() {
//    if (is_show_setting == true) return;
//    if (is_busy == true) return;
//    is_busy = true;
//    $("#menubackground").stop(true, true).slideDown(100, function() {
//        $("#menumove").stop(true, true).fadeIn(100, function() {
//            is_show_setting = true;
//            is_busy = false;
//        });
//    });
}
function hideSetting() {
//    if (is_show_setting == false) return;
//    $("#submenu_pen").stop(true, true).slideUp("fast", function() {
//        $("#submenu_marker").stop(true, true).slideUp("fast", function() {
//            $("#submenu_eraser").stop(true, true).slideUp("fast", function() {
//                $("#menumove").stop(true, true).fadeOut("fast", function() {
//                    $("#menubackground").stop(true, true).slideUp("fast", function() {
//                        $(".moveup").slideDown("fast", function() {
//                            is_show_setting = false;
//                        });
//                    });
//                });
//                is_show_setting_eraser = false;
//            });
//            is_show_setting_marker = false;
//        });
//        is_show_setting_pen = false;
//    });
}
function showSettingPen() {
//    if (scale_zoom != 1.0) return; //ズーム中は処理しない
//    $("#submenu_pen").stop(true, true).slideDown("fast", function() { is_show_setting_pen = true; });
}
function hideSettingPen() {
//    $("#submenu_pen").stop(true, true).slideUp("fast", function() { is_show_setting_pen = false; });
}
function showSettingMarker() {
//    if (scale_zoom != 1.0) return; //ズーム中は処理しない
//    $("#submenu_marker").stop(true, true).slideDown("fast", function() { is_show_setting_marker = true; });
}
function hideSettingMarker() {
//    $("#submenu_marker").stop(true, true).slideUp("fast", function() { is_show_setting_marker = false; });
}
function zoomScaleChange(scale){
    scale_zoom = scale;
}

function zoomScaleEnd(){
    scale_zoom = 1.0
}

$(".pen_width").click(
    function() {
        document.getElementById("draw").style.display = "block";

        width_pen = parseInt($(this).attr("id").slice(-1));
		if(width_pen == 1){
			canvas.freeDrawingBrush.width = 2;
		}
		else if(width_pen == 2){
			canvas.freeDrawingBrush.width = 4;
		}
		else if(width_pen == 3){
			canvas.freeDrawingBrush.width = 8;
		}
		else if(width_pen == 4){
			canvas.freeDrawingBrush.width = 16;
		}
		else if(width_pen == 5){
			canvas.freeDrawingBrush.width = 25;
		}

//        if (width_pen == 1) {
//            $("#pen_width_1").attr('src','images/SET_line1_S.png');
//            $("#pen_width_2").attr('src','images/SET_line2_P.png');
//            $("#pen_width_3").attr('src','images/SET_line3_P.png');
//        } else if (width_pen == 2) {
//            $("#pen_width_1").attr('src','images/SET_line1_P.png');
//            $("#pen_width_2").attr('src','images/SET_line2_S.png');
//            $("#pen_width_3").attr('src','images/SET_line3_P.png');
//        } else if (width_pen == 3) {
//            $("#pen_width_1").attr('src','images/SET_line1_P.png');
//            $("#pen_width_2").attr('src','images/SET_line2_P.png');
//            $("#pen_width_3").attr('src','images/SET_line3_S.png');
//        }

        selectItemType(ITEM_TYPE.LINE_FREE_PEN);
        hideSettingPen();
    }
);
$(".pen_color").click(
    function() {
        document.getElementById("draw").style.display = "block";

        $("#pen_black").attr('src','images/SET_color_BLACK_P.png');
        $("#pen_red").attr('src','images/SET_color_RED_P.png');
        $("#pen_green").attr('src','images/SET_color_GREEN_P.png');
        $("#pen_blue").attr('src','images/SET_color_BLUE_P.png');
        $("#pen_yellow").attr('src','images/SET_color_YELLOW_P.png');
        $("#pen_magenta").attr('src','images/SET_color_MAGENTA_P.png');
        $("#pen_cyan").attr('src','images/SET_color_CYAN_P.png');
        $("#pen_white").attr('src','images/SET_color_WHITE_P.png');

        //クリックされた色を取得
        var color = $(this).attr("id");
        if (color == "pen_black") {
            color_pen = "rgba(0,0,0,1.0)";
            $("#pen_black").attr('src','images/SET_color_BLACK_S.png');
        } else if (color == "pen_red") {
            color_pen = "rgba(255,0,0,1.0)";
            $("#pen_red").attr('src','images/SET_color_RED_S.png');
        } else if (color == "pen_green") {
            color_pen = "rgba(0,255,0,1.0)";
            $("#pen_green").attr('src','images/SET_color_GREEN_S.png');
        } else if (color == "pen_blue") {
            color_pen = "rgba(0,0,255,1.0)";
            $("#pen_blue").attr('src','images/SET_color_BLUE_S.png');
        } else if (color == "pen_yellow") {
            color_pen = "rgba(255,255,0,1.0)";
            $("#pen_yellow").attr('src','images/SET_color_YELLOW_S.png');
        } else if (color == "pen_magenta") {
            color_pen = "rgba(255,0,255,1.0)";
            $("#pen_magenta").attr('src','images/SET_color_MAGENTA_S.png');
        } else if (color == "pen_cyan") {
            color_pen = "rgba(0,255,255,1.0)";
            $("#pen_cyan").attr('src','images/SET_color_CYAN_S.png');
        } else if (color == "pen_white") {
            color_pen = "rgba(255,255,255,1.0)";
            $("#pen_white").attr('src','images/SET_color_WHITE_S.png');
        }
        if (item_type == ITEM_TYPE.LINE_FREE_PEN) {
            canvas.freeDrawingBrush.color = color_pen;
        } else {
            canvas.freeDrawingBrush.color = color_pen.slice(0,-4) + "0.0)";
        }

        selectItemType(ITEM_TYPE.LINE_FREE_PEN);
        hideSettingPen();
    }
);
function showSettingMarker() {
//    if (scale_zoom != 1.0) return; //ズーム中は処理しない
//    $("#submenu_marker").stop(true, true).slideDown("fast", function() { is_show_setting_marker = true; });
}
function hideSettingMarker() {
//    $("#submenu_marker").stop(true, true).slideUp("fast", function() { is_show_setting_marker = false; });
}
$(".marker_opacity").click(
    function() {
        document.getElementById("draw").style.display = "block";

        var opacity_id = $(this).attr("id");
        var opacity_str = opacity_id.replace("opacity_", "");
        opacity_marker = parseFloat(opacity_str , 10) / 10.0;
        canvas.freeDrawingBrush.color = canvas.freeDrawingBrush.color.slice(0,-4) + opacity_marker.toFixed(1) + ")";

//        if (opacity_marker.toFixed(1) == 1.0) {
//            $("#marker_opacity_10").attr('src','images/SET_PEN_S.png');
//            $("#marker_opacity_5").attr('src','images/SET_MARKER_P.png');
//        } else {
//            $("#marker_opacity_10").attr('src','images/SET_PEN_P.png');
//            $("#marker_opacity_5").attr('src','images/SET_MARKER_S.png');
//        }

        selectItemType(ITEM_TYPE.LINE_FREE_MARKER);
        hideSettingMarker();
    }
);
$(".marker_width").click(
    function() {
        document.getElementById("draw").style.display = "block";

        width_marker = parseInt($(this).attr("id").slice(-1));
		if(width_marker == 1){
			canvas.freeDrawingBrush.width = 2;
		}
		else if(width_marker == 2){
			canvas.freeDrawingBrush.width = 4;
		}
		else if(width_marker == 3){
			canvas.freeDrawingBrush.width = 8;
		}
		else if(width_marker == 4){
			canvas.freeDrawingBrush.width = 16;
		}
		else if(width_marker == 5){
			canvas.freeDrawingBrush.width = 25;
		}
		
//        if (width_marker == 1) {
//            $("#marker_width_1").attr('src','images/SET_line1_S.png');
//            $("#marker_width_2").attr('src','images/SET_line2_P.png');
//            $("#marker_width_3").attr('src','images/SET_line3_P.png');
//        } else if (width_marker == 2) {
//            $("#marker_width_1").attr('src','images/SET_line1_P.png');
//            $("#marker_width_2").attr('src','images/SET_line2_S.png');
//            $("#marker_width_3").attr('src','images/SET_line3_P.png');
//        } else if (width_marker == 3) {
//            $("#marker_width_1").attr('src','images/SET_line1_P.png');
//            $("#marker_width_2").attr('src','images/SET_line2_P.png');
//            $("#marker_width_3").attr('src','images/SET_line3_S.png');
//        }

        selectItemType(ITEM_TYPE.LINE_FREE_MARKER);
        hideSettingMarker();
    }
);
$(".marker_color").click(
    function() {
        document.getElementById("draw").style.display = "block";

        $("#marker_black").attr('src','images/SET_color_BLACK_P.png');
        $("#marker_red").attr('src','images/SET_color_RED_P.png');
        $("#marker_green").attr('src','images/SET_color_GREEN_P.png');
        $("#marker_blue").attr('src','images/SET_color_BLUE_P.png');
        $("#marker_yellow").attr('src','images/SET_color_YELLOW_P.png');
        $("#marker_magenta").attr('src','images/SET_color_MAGENTA_P.png');
        $("#marker_cyan").attr('src','images/SET_color_CYAN_P.png');
        $("#marker_white").attr('src','images/SET_color_WHITE_P.png');

        //クリックされた色を取得
        var color = $(this).attr("id");
        if (color == "marker_black") {
            color_marker = "rgba(0,0,0," + opacity_marker.toFixed(1) + ")";
            $("#marker_black").attr('src','images/SET_color_BLACK_S.png');
        } else if (color == "marker_red") {
            color_marker = "rgba(255,0,0," + opacity_marker.toFixed(1) + ")";
            $("#marker_red").attr('src','images/SET_color_RED_S.png');
        } else if (color == "marker_green") {
            color_marker = "rgba(0,255,0," + opacity_marker.toFixed(1) + ")";
            $("#marker_green").attr('src','images/SET_color_GREEN_S.png');
        } else if (color == "marker_blue") {
            color_marker = "rgba(0,0,255," + opacity_marker.toFixed(1) + ")";
            $("#marker_blue").attr('src','images/SET_color_BLUE_S.png');
        } else if (color == "marker_yellow") {
            color_marker = "rgba(255,255,0," + opacity_marker.toFixed(1) + ")";
            $("#marker_yellow").attr('src','images/SET_color_YELLOW_S.png');
        } else if (color == "marker_magenta") {
            color_marker = "rgba(255,0,255," + opacity_marker.toFixed(1) + ")";
            $("#marker_magenta").attr('src','images/SET_color_MAGENTA_S.png');
        } else if (color == "marker_cyan") {
            color_marker = "rgba(0,255,255," + opacity_marker.toFixed(1) + ")";
            $("#marker_cyan").attr('src','images/SET_color_CYAN_S.png');
        } else if (color == "marker_white") {
            color_marker = "rgba(255,255,255," + opacity_marker.toFixed(1) + ")";
            $("#marker_white").attr('src','images/SET_color_WHITE_S.png');
        }
        if (item_type == ITEM_TYPE.LINE_FREE_MARKER) {
            canvas.freeDrawingBrush.color = color_marker;
        } else {
            canvas.freeDrawingBrush.color = color_marker.slice(0,-4) + "0.0)";
        }

        selectItemType(ITEM_TYPE.LINE_FREE_MARKER);
        hideSettingMarker();
    }
);
function showSettingEraser() {
//    if (scale_zoom != 1.0) return; //ズーム中は処理しない
//    $("#submenu_eraser").stop(true, true).slideDown("fast", function() { is_show_setting_eraser = true; });
}
function hideSettingEraser() {
//    $("#submenu_eraser").stop(true, true).slideUp("fast", function() { is_show_setting_eraser = false; });
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//メニュー
///////////////////////////////////////////////////////////////////////////////////////////////////
//その他
$(document).on('click',function(e) {
   if(!$(e.target).closest('.pencil').length) {
       hideSettingPen();
   }
   if(!$(e.target).closest('.clearObject').length) {
       hideSettingEraser();
   }
});
//ペン
$(".pencil").click(function(e) {
//    if (scale_zoom != 1.0) return; //ズーム中は処理しない
    canvas.freeDrawingBrush.width = width_pen; //自由線幅
    canvas.freeDrawingBrush.color = color_pen;
    document.getElementById("draw").style.display = "block";
    selectItemType(ITEM_TYPE.LINE_FREE_PEN);
    if (is_show_setting_pen == false) {
        hideSettingMarker();
        hideSettingEraser();
        showSettingPen();
    } else {
        hideSettingPen();
    }
});
//マーカー
$(".marker").click(function(e) {
//    if (scale_zoom != 1.0) return; //ズーム中は処理しない
    canvas.freeDrawingBrush.width = width_marker; //自由線幅
    canvas.freeDrawingBrush.color = color_marker.slice(0,-4) + opacity_marker.toFixed(1) + ")";
    document.getElementById("draw").style.display = "block";
    selectItemType(ITEM_TYPE.LINE_FREE_MARKER);
    if (is_show_setting_marker == false) {
        hideSettingPen();
        hideSettingEraser();
        showSettingMarker();
    } else {
        hideSettingMarker();
    }
});
//部分消去
$('.clearObject').on('click', function(e) {
//    if (scale_zoom != 1.0) return; //ズーム中は処理しない
    document.getElementById("draw").style.display = "block";
    selectItemType(ITEM_TYPE.ERASER);
    if (is_show_setting_eraser == false) {
        hideSettingPen();
        hideSettingMarker();
        showSettingEraser();
    } else {
        hideSettingEraser();
    }
});
//全消去
$('.clearAll').on('click', function(e) {
    $('#dom').off('click'); $('#dom').on('click', clear(e));
    document.getElementById("draw").style.display = "block";
    hideSettingEraser();
});
//選択
$(".selectitem").click(function() {
    if (background_type == "teaching_material") {
        $(".clearAll").trigger('click'); //描画内容クリア
        document.getElementById("draw").style.display = "none"; //直接さわれるようにcanvas非表示
    } else {
        document.getElementById("draw").style.display = "block";
    }
    selectItemType(ITEM_TYPE.SELECT);
//    selectItemType(ITEM_TYPE.ZOOM); //オブジェクト選択ではなくズームの移動用に使用する
});
//書画モード
$('#cameramode').click(function(e) {
//    changeBackgroundBefore("camera"); //背景変更前処理
//    r = 0; startVideo(camera_w, camera_h, camera_fr, true); //初期は720P
//    changeBackgroundAfter("camera", false); //背景変更後処理
    /* 書画モードへの遷移はchangeModeCamera(deviceid)を使用する */
});
//ホワイトモード1
$('#wbmode1').on('click', function(e) {
    changeModeWhiteboard("whiteboard1");
});
//ホワイトモード2
$('#wbmode2').on('click', function(e) {
    changeModeWhiteboard("whiteboard2");
});
//ホワイトモード3
$('#wbmode3').on('click', function(e) {
    changeModeWhiteboard("whiteboard3");
});
//ホワイトモード4
$('#wbmode4').on('click', function(e) {
    changeModeWhiteboard("whiteboard4");
});
//透明
$('#transparent').on('click', function(e) {
    changeBackgroundBefore("transparent"); //背景変更前処理
    changeBackgroundAfter("transparent", false); //背景変更後処理
});
//コンテンツモード
$("#contentsmode").click(function () {
    setTimeout(function(){ $(selectContents).click(); },200);
});
//キャプチャ
$('.captureall').on('click', function(e) {
    download_image(0, 0, 0, 0);
});
//範囲指定キャプチャ
//$('.capturearea').on('click', function(e) {
//    selectAreaCapture();
//});
//録画
$('.recording').on('click', function(e) {
    //if (localStream == null) return;
    if (is_recording == false) {
        startRecording();
    } else {
        stopRecording();
    }
});
//フリーズ
$('.freeze').on('click', function(e) {
    freeze();
});
//回転
$('.rotation').on('click', function(e) {
    rotation(true);
});
//画像挿入
//$('.insertimage').on('click', function(e) {
//    insertimage("");
//});
//アップロード
//$('.upload').on('click', function(e) {
//    setTimeout(function(){ $(selectUpload).click(); },200);
//});
//文字入力
//$('.text').on('click', function(e) {
//    selectItemType(ITEM_TYPE.TEXT);
//});
function setPropertyText(in_font_size, in_font_family, in_font_weight, in_color_text, in_color_text_background) {
    if (in_font_size > 0) font_size_text = in_font_size;
    if (in_font_family != null) font_family_text = in_font_family;
    if (in_font_weight != null) font_weight_text = in_font_weight;
    if (in_color_text != null) color_text = in_color_text;
    if (in_color_text_background != null) color_text_background = in_color_text_background;
}
//メニュー位置変更
var menuposition_left = true;
$('.positionchange').on('click', function(e) {
    $('#footer').fadeOut("fast");
    $('#menulist').fadeOut("fast", function() {
        if (menuposition_left == true) {
            menuposition_left = false;
            $('.positionchange').attr('src', 'images/Move_L_S.png');
            $('.menudown').attr('src', 'images/Close_R_S.png');
            $('#menulist').css('float', 'right');
            $('#menulist').css('margin-right','0px');
            $('#menulist li').css('float', 'right');
            $('#menulist li').css('margin-right','0px');
            $('#menulist li').css('margin-left','0px');
            $('#menuhide').css('margin-right','0px');
            $('#menuhide').css('margin-left','0px');
            $('#menupos').css('float','left');
            $('#menupos').css('margin-left','-40px');
            $('#pen_style').children().each(function(i,li){$('#pen_style').prepend(li)})
            $('#pen_style').css('float', 'right');
            $('#pen_style').css('margin-right','30px');
            $('#marker_style').children().each(function(i,li){$('#marker_style').prepend(li)})
            $('#marker_style').css('float', 'right');
            $('#marker_style').css('margin-right','100px');
            $('#eraser_style').css('float', 'right');
            $('#eraser_style').css('margin-right','180px');
            $('#menulist').fadeIn("fast");
            $('#footer').fadeIn("fast");
        } else {
            menuposition_left = true;
            $('.positionchange').attr('src', 'images/Move_R_S.png');
            $('.menudown').attr('src', 'images/Close_L_S.png');
            $('#menulist').css('float', 'left');
            $('#menulist').css('margin-left','0px');
            $('#menulist li').css('float', 'left');
            $('#menulist li').css('margin-right','0px');
            $('#menulist li').css('margin-left','0px');
            $('#menuhide').css('margin-left','-40px');
            $('#menuhide').css('margin-right','10px');
            $('#menupos').css('float','right');
            $('#menupos').css('margin-right','0px');
            $('#pen_style').children().each(function(i,li){$('#pen_style').prepend(li)})
            $('#pen_style').css('float', 'left');
            $('#pen_style').css('margin-left','0px');
            $('#marker_style').children().each(function(i,li){$('#marker_style').prepend(li)})
            $('#marker_style').css('float', 'left');
            $('#marker_style').css('margin-left','70px');
            $('#eraser_style').css('float', 'left');
            $('#eraser_style').css('margin-left','140px');
            $('#menulist').fadeIn("fast");
            $('#footer').fadeIn("fast");
        }
    });
});
//メニュー表示
$(".menuup").click(function(e) {
    $(".moveup").slideUp("fast", function() {
        showSetting();
    });
});
//メニュー非表示
$('.menudown').on('click', function(e) {
    hideSetting();
});
//フルスクリーン
var is_fullscreen = false;
$('.fullscreen').on('click', function(e) {
    if (is_fullscreen == false) {
        $(".fullscreen").attr('src','images/FullScreen_OFF.png');
        is_fullscreen = true;
        $('#fullscreen').fullscreen();
    } else {
        $(".fullscreen").attr('src','images/FullScreen.png');
        is_fullscreen = false;
        $.fullscreen.exit();
    }
});
//ヘルプ
$('.help').on('click', function(e) {
//    changeModeMask(100, 100, 300, 500, "rgba(0,0,0,0.5)");
//    changeModeHighlight(100, 100, 300, 500, "rgba(0,0,0,0.5)");
//    if (list_selecteditem_compare.length == 0) {
//        changeModeCompare(["file:///C:/Users/Public/Pictures/Sample Pictures/Chrysanthemum.jpg", "file:///C:/Users/Public/Pictures/Sample Pictures/Hydrangeas.jpg", "file:///C:/Users/Public/Pictures/Sample Pictures/Jellyfish.jpg", "file:///C:/Users/Public/Pictures/Sample Pictures/Koala.jpg", "file:///C:/Users/Public/Pictures/Sample Pictures/Lighthouse.jpg", "file:///C:/Users/Public/Pictures/Sample Pictures/Penguins.jpg", "file:///C:/Users/Public/Pictures/Sample Pictures/Tulips.jpg"]);
//    } else {
//        showSelectedItemCompare();
//    }
});
//マスク
function changeModeMask(x, y, w, h, color) { //colorはマスクの色
    if (x < 0 || y < 0 || w < 0 || h < 0 || color == "") {
        console.log("mask param is invalid.");
        return false;
    }
    if (is_mask == false) {
        is_mask = true;

        item_mask = new fabric.Rect({
            left: x,
            top: y,
            originX: 'left',
            originY: 'top',
            width: w,
            height: h,
            angle: 0,
            strokeWidth : 1,
            stroke: color,
            fill: color
        });
        item_mask.lockRotation = true;
        item_mask.hasRotatingPoint = false;
        item_mask.setCoords();

        canvas.add(item_mask);
        canvas.renderAll();
    } else {
        is_mask = false;
        for (var i = canvas._objects.length - 1; i >= 0; i--) {
            if (canvas._objects[i] == item_mask) {
                canvas.remove(item_mask);
                break;
            }
            canvas.remove(canvas._objects[i]);
        }
    }
    return true;
}
function changeModeMaskColor(color) { //色のみ
    if (color == "") {
        console.log("color param is invalid.");
        return false;
    }
    item_mask.set({ stroke: color, fill: color });
    canvas.renderAll(); //更新
    return true;
}
//ハイライト
function changeModeHighlight(x, y, w, h, color) { //colorはハイライトの暗い部分の色
    if (x < 0 || y < 0 || w < 0 || h < 0 || color == "") {
        console.log("highlight param is invalid.");
        return false;
    }
    if (is_highlight == false) {
        is_highlight = true;

        item_highlight = new fabric.Rect({
            left: x,
            top: y,
            originX: 'left',
            originY: 'top',
            width: w,
            height: h,
            angle: 0,
            strokeWidth : 0,
            fill: "rgba(0,0,0,0.01)"
        });
        item_highlight.lockRotation = true;
        item_highlight.hasRotatingPoint = false;
        item_highlight.setCoords();

        var x_bg = canvas.width % -2 == 0 ? Math.ceil(canvas.width / -2) : canvas.width / -2;
        var y_bg = canvas.height % -2 == 0 ? Math.ceil(canvas.height / -2) : canvas.height / -2;
        item_highlight_bg_t.set({ width: canvas.width, height: canvas.height, stroke: color, fill: color, clipTo: function (ctx) { ctx.rect(x_bg, y_bg, canvas.width, item_highlight.top); } });
        item_highlight_bg_b.set({ width: canvas.width, height: canvas.height, stroke: color, fill: color, clipTo: function (ctx) { ctx.rect(x_bg, y_bg + item_highlight.top + h, canvas.width, canvas.height - item_highlight.top - h); } });
        item_highlight_bg_l.set({ width: canvas.width, height: canvas.height, stroke: color, fill: color, clipTo: function (ctx) { ctx.rect(x_bg, y_bg + item_highlight.top, item_highlight.left, h); } });
        item_highlight_bg_r.set({ width: canvas.width, height: canvas.height, stroke: color, fill: color, clipTo: function (ctx) { ctx.rect(x_bg + item_highlight.left + w, y_bg + item_highlight.top, canvas.width - item_highlight.left - w, h); } });

        canvas.add(item_highlight_bg_t);
        canvas.add(item_highlight_bg_b);
        canvas.add(item_highlight_bg_l);
        canvas.add(item_highlight_bg_r);
        canvas.add(item_highlight);
        canvas.renderAll(); //更新
    } else {
        is_highlight = false;
        for (var i = canvas._objects.length - 1; i >= 0; i--) {
            if (canvas._objects[i] == item_highlight_bg_t) {
                canvas.remove(item_highlight_bg_t);
                break;
            }
            canvas.remove(canvas._objects[i]);
        }
    }
    return true;
}
function changeModeHighlightColor(color) { //色のみ
    if (color == "") {
        console.log("color param is invalid.");
        return false;
    }
    item_highlight_bg_t.set({ stroke: color, fill: color });
    item_highlight_bg_b.set({ stroke: color, fill: color });
    item_highlight_bg_l.set({ stroke: color, fill: color });
    item_highlight_bg_r.set({ stroke: color, fill: color });
    canvas.renderAll(); //更新
    return true;
}
//コンペア
function changeModeCompare(list) {
    if (list.length < 1 || 9 < list.length) {
        console.log("compare param is invalid.");
        return false;
    }
    if (is_compare == false) {
        is_compare = true;
        clearListCompare(); //リストクリア
        setListCompare(list); //リストセット
    } else {
        is_compare = false;
        $(".clearAll").trigger('click'); //解除時は描画内容クリア
    }
    return true;
}
function clearListCompare() {
    list_image_compare = []; //画像比較リストクリア
    list_item_compare = [];
    list_path_compare = [];
    list_selecteditem_compare = [];
}
function setListCompare(list_path) { //外部からパスリストを送って表示する
    var length = list_image_compare.length + list_path.length;
    for (var i = 0; i < list_path.length; i++) {
        if (list_path_compare.indexOf(list_path[i]) < 0) {
            list_path_compare.push(list_path[i]);
        } else {
            length--;
            continue;
        }
        loadImageItemFromPath(length - list_path.length + i, length, list_path[i]);
    }
    setTimeout(function() {
        var list = [];
        for (var i = 0; i < list_path_compare.length; i++) {
            for (var j = 0; j < list_image_compare.length; j++) {
                var img = list_image_compare[j];
                if (list_path_compare[i].includes(img.cacheKey)) {
                    list.splice(i, 0, img);
                }
            }
        }
        list_image_compare = list;
        relocateImageItem(length); //既に配置されている分を再配置
    }, list_path.length * 10);
}
function loadImageItemFromPath(i, length, path) {
    fabric.Image.fromURL(path, function(img) {
        if (i < 9) {
            img.set({ cacheKey: path }); //cacheKeyを使っていないのでファイルパスを入れておく
            list_image_compare.push(img);
        }
    });
}
function getListSelectedItemCompare() { //選択中のファイル名リスト取得
    var list = [];
    for (var i = 0; i < list_selecteditem_compare.length; i++) {
        list.push(list_selecteditem_compare[i].getObjects()[0].cacheKey);
    }
    return list;
}
function showSelectedItemCompare() {
    var list = getListSelectedItemCompare();
    if (list.length == 0) return;
    is_compare = false;
    $(".clearAll").trigger('click'); //解除時は描画内容クリア
    changeModeCompare(list);
}
/*
///////////////////////////////////////////////////////////////////////////////////////////////////
//Google Vision
///////////////////////////////////////////////////////////////////////////////////////////////////
const api_key = ``; //Google Developerで発行したAPIキー
const url = `https://vision.googleapis.com/v1/images:annotate`;
var recognition_type = 'FACE_DETECTION';
// Send API Request to Cloud Vision API
const sendAPI = (base64string) => {
    let body = {
        requests: [
          {image: {content: base64string}, features: [{type: recognition_type}]}
        ]
    };
    let xhr = new XMLHttpRequest();
    xhr.open('POST', `${url}?key=${api_key}`, true);
    xhr.setRequestHeader('Content-Type', 'application/json');
    const p = new Promise((resolve, reject) => {
    xhr.onreadystatechange = () => {
        if (xhr.readyState != XMLHttpRequest.DONE) return;
        if (xhr.status >= 400) return reject({message: `Failed with ${xhr.status}:${xhr.statusText}`});
        resolve(JSON.parse(xhr.responseText));
    };
    });
    xhr.send(JSON.stringify(body));
    return p;
};
// Read input file
const readFile = (file) => {
    let reader = new FileReader();
    const p = new Promise((resolve, reject) => {
        reader.onload = (ev) => {
            resolve(ev.target.result.replace(/^data:image\/(png|jpeg);base64,/, ''));
        };
    });
    reader.readAsDataURL(file);
    return p;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//Google Drive
///////////////////////////////////////////////////////////////////////////////////////////////////
var googleDriveClient;
getGoogleAuth().then(loadGoogleDrive).then(function(gClient) {
    googleDriveClient = gClient;
});
// google driveのクライアントの生成
function loadGoogleDrive() {
    var p = new Promise(function(resolve, reject) {
        try {
            window.gapi.client.load('drive', 'v3', fncOnDriveApiLoad);
        } catch (e) {
            reject(e);
        }
        function fncOnDriveApiLoad() {
            resolve(window.gapi.client);
        }
    });
    return p;
}
// Google認証
function getGoogleAuth() {
    var objAuthParam = {
        // クライアントIDはGoogle API consoleで取得してください
        'client_id': gClientId,
        'scope': ['https://www.googleapis.com/auth/drive'],
        'immediate': false
    };
    var p = new Promise(function(resolve, reject) {
        window.gapi.load('auth', {
            'callback': function () {
                window.gapi.auth.authorize(
                    objAuthParam,
                    authResult);
            }
        });
        function authResult(objAuthResult) {
            if (objAuthResult && !objAuthResult.error) {
                resolve(objAuthResult.access_token);
            } else {
                // auth failed.
                reject(objAuthResult);
            }
        }
    });
    return p;
}
// ファイルアップロード
function uploadFile(gClient, base64FileData, fileName, fileType, callback) {
    // 固定文
    const boundary = '-------314159265358979323846';
    const delimiter = '\r\n--' + boundary + '\r\n';
    const closeDelim = '\r\n--' + boundary + '--';
    const contentType = fileType;

    // アップロード先のフォルダ指定など
    var metadata = {
        'name': fileName,
        'mimeType': contentType
    };
    var multipartRequestBody =
        delimiter +
        'content-type: application/json; charset=UTF-8\r\n\r\n' +
        JSON.stringify(metadata) +
        delimiter +
        'content-transfer-encoding: base64\r\n' +
        'content-type: ' + contentType + '\r\n\r\n' +
        base64FileData +
        closeDelim;
    var request = gClient.request({
        'path': '/upload/drive/v3/files',
        'method': 'POST',
        'params': {
            'uploadType': 'multipart'
        },
        'headers': {
            'Content-Type': 'multipart/related; boundary="' + boundary + '"'
        },
        'body': multipartRequestBody
    });
    try {
        request.execute(function (objFile) {
            console.log(objFile);
            console.info('upload success');
        });
    } catch (e) {
        console.error(e);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//Google Vision 顔認識
///////////////////////////////////////////////////////////////////////////////////////////////////
function facerecognition() {
    freeze(); //画像取得
    var blob = base64ToBlob(canvas.toDataURL());
    //笑顔認識
    recognition_type = 'FACE_DETECTION';
    Promise.resolve(blob)
    .then(readFile)
    .then(sendAPI)
    .then(res => {
        //console.log('SUCCESS!', res);
        //console.log(JSON.stringify(res, null, 2));
        var res_json = JSON.stringify(res, null, 2).toString();
        //表情判定
        if (res_json.includes("\"joyLikelihood\": \"VERY_LIKELY\"")) { //笑顔
        } else if (res_json.includes("\"joyLikelihood\": \"LIKELY\"") || res_json.includes("\"joyLikelihood\": \"POSSIBLE\"")) { //微笑
        } else if (res_json.includes("\"sorrowLikelihood\": \"VERY_LIKELY\"")) { //泣く
        } else if (res_json.includes("\"angerLikelihood\": \"VERY_LIKELY\"")) { //怒り
        } else if (res_json.includes("\"surpriseLikelihood\": \"VERY_LIKELY\"")) { //驚き
        } else { //普通
        }
    })
    .catch(err => {
        console.log('FAILED:(', err);
    });
    freeze(); //再開
};
*/
///////////////////////////////////////////////////////////////////////////////////////////////////
//全消去
///////////////////////////////////////////////////////////////////////////////////////////////////
function clear(e) {
    e.preventDefault(); //デフォルトイベントをキャンセル

    canvas.clear(); //描画内容も背景も全てクリア
    if (background_type == "camera" || background_type == "video" || background_type == "transparent") {
        canvas.backgroundColor = "rgba(0, 0, 0, 0.0)"; //透明背景
    } else if (background_type.includes("whiteboard")) {
        if (background_type == "whiteboard1") {
            drawBackground(imgDefaultTmp1);
        } else if (background_type == "whiteboard2") {
            drawBackground(imgDefaultTmp2);
        } else if (background_type == "whiteboard3") {
            drawBackground(imgDefaultTmp3);
        } else if (background_type == "whiteboard4") {
            drawBackground(imgDefaultTmp4);
        }
    } else if (background_type == "image") {
        drawBackground(imgBackgroundTmp);
    }
    if (is_mask == true) {
        canvas.add(item_mask);
        canvas.renderAll();
    }
    if (is_highlight == true) {
        canvas.add(item_highlight_bg_t);
        canvas.add(item_highlight_bg_b);
        canvas.add(item_highlight_bg_l);
        canvas.add(item_highlight_bg_r);
        canvas.add(item_highlight);
        canvas.renderAll();
    }
    if (is_compare == true) {
        relocateImageItem(list_image_compare.length);
    }

    $(".pencil").trigger('click'); //描画クリアしたら気を利かせてペンにする
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//モード変更
///////////////////////////////////////////////////////////////////////////////////////////////////
var mode_current = MODE.WHITEBOARD1;
function changeMode() {
    obj = document.modeList.selectedMode;
    var index = obj.selectedIndex;
    var mode = obj.options[index].value;
    if (mode == MODE.WHITEBOARD1) { //ホワイトボード1
        $('#wbmode1').trigger('click');
        mode_current = mode;
        $('.recording').fadeOut("fast");
        $(".rotation").fadeOut("fast");
        $(".freeze").fadeOut("fast");
    } else if (mode == MODE.WHITEBOARD2) { //ホワイトボード2
        $('#wbmode2').trigger('click');
        mode_current = mode;
        $('.recording').fadeOut("fast");
        $(".rotation").fadeOut("fast");
        $(".freeze").fadeOut("fast");
    } else if (mode == MODE.WHITEBOARD3) { //ホワイトボード3
        $('#wbmode3').trigger('click');
        mode_current = mode;
        $('.recording').fadeOut("fast");
        $(".rotation").fadeOut("fast");
        $(".freeze").fadeOut("fast");
    } else if (mode == MODE.WHITEBOARD4) { //ホワイトボード4
        $('#wbmode4').trigger('click');
        mode_current = mode;
        $('.recording').fadeOut("fast");
        $(".rotation").fadeOut("fast");
        $(".freeze").fadeOut("fast");
    } else if (mode == MODE.TRANSPARENT) { //透明背景
        $('#transparent').trigger('click');
        mode_current = mode;
        $('.recording').fadeOut("fast");
        $(".rotation").fadeOut("fast");
        $(".freeze").fadeOut("fast");
    } else if (mode == MODE.CONTENTS) { //コンテンツ
        $('select[name=selectedMode]').val(mode_current);
        $('#modeList').selectpicker("refresh"); //リスト更新
        $('#contentsmode').trigger('click');
    } else if (mode == MODE.IMAGE) { //静止画背景
        changeModeImage("");
        $('.recording').fadeOut("fast");
        $(".rotation").fadeIn("fast");
        $(".freeze").fadeOut("fast");
    } else if (mode == MODE.VIDEO) { //動画背景
        changeModeVideo("");
        $('.recording').fadeOut("fast");
        $(".rotation").fadeIn("fast");
        $(".freeze").fadeIn("fast");
    } else if (mode == MODE.EDUCATIONAL_MATERIAL) { //教材背景
        var path = prompt(TEXT_INPUT_EDUCATIONAL_MATERIAL_PATH, "");
        if ((path == null || path == "") || (USE_LOCAL == false && !path.includes("http"))) {
            if (tmBackgroundTmp == null) {
                $('select[name=selectedMode]').val(MODE.WHITEBOARD1);
                $('#modeList').selectpicker("refresh"); //リスト更新
                changeMode(); //ホワイトボードモードに遷移
                return;
            } else {
                path = "";
            }
        }
        changeModeEducationalMaterial(path);
        $('.recording').fadeOut("fast");
        $(".rotation").fadeOut("fast");
        $(".freeze").fadeOut("fast");
    } else { //その他はカメラとして処理する
        changeModeCamera(mode);
    }
}
$('#mode').on('change', function() {
    changeMode();
}).on('show.bs.dropdown', function () {
    hideSettingPen();
    hideSettingMarker();
    hideSettingEraser();
    if (useBrowser == "safari_not_support_webrtc") {
    } else {
        navigator.mediaDevices.enumerateDevices().then(gotDevices).catch(errorCallback); //途中でカメラ接続されたときのためにリスト表示時に一回読み込む
    }
});

///////////////////////////////////////////////////////////////////////////////////////////////////
//背景変更前処理 / 背景変更後処理
///////////////////////////////////////////////////////////////////////////////////////////////////
function changeBackgroundBefore(type)
{
    //設定解除
    if (is_recording == true) { //録画中なら停止する
        alert(TEXT_RECORDING_END);
        stopRecording();
    }
    if (localStream != null) stopVideo(); //ライブ停止
    if (is_freeze == true) freeze(); //背景切替するのでフリーズを解除
    if (angle_rotation == 180) rotation(false); //背景切替するので回転を解除
    if (is_mask == true) changeModeMask(0, 0, 0, 0, "rgba(0, 0, 0, 0)"); //マスク解除(引数は意味ないので適当)
    if (is_highlight == true) changeModeHighlight(0, 0, 0, 0, "rgba(0, 0, 0, 0)"); //マスク解除(引数は意味ないので適当)
    if ($("input[type=range]").val() != 1.0) $("input[type=range]").val(1.0).change(); //ズーム解除

    //描画内容切替
    clearObjectsCanvas(background_type); //描画内容保存の前にクリアする
    saveObjectsCanvas(background_type); //描画内容保存
    canvas.clear(); //描画内容も背景も全てクリア
}
function changeBackgroundBeforeTrans(type){
    if (localStream != null) stopVideo(); //ライブ停止
    if (is_freeze == true) freeze(); //背景切替するのでフリーズを解除
    if (angle_rotation == 180) rotation(false); //背景切替するので回転を解除
    if (is_mask == true) changeModeMask(0, 0, 0, 0, "rgba(0, 0, 0, 0)"); //マスク解除(引数は意味ないので適当)
    if (is_highlight == true) changeModeHighlight(0, 0, 0, 0, "rgba(0, 0, 0, 0)"); //マスク解除(引数は意味ないので適当)
    if ($("input[type=range]").val() != 1.0) $("input[type=range]").val(1.0).change(); //ズーム解除

    saveObjectsCanvas(background_type); //描画内容保存
    canvas.clear(); //描画内容も背景も全てクリア
    canvas.backgroundColor = "rgba(0, 0, 0, 0.0)"; //透明背景
    loadObjectsCanvas(type); //描画内容を読み込む
}

function changeBackgroundAfter(type, clear_objs)
{
    if (type == "camera" || type == "video") {
        $("#live").css('background-color', 'black'); //video黒背景
        document.getElementById("video").style.display = "block"; //video表示
    } else {
        $("#live").css('background-color', 'transparent'); //video透明背景
        document.getElementById("video").style.display = "none"; //video非表示
    }
    if (clear_objs == true) {
        clearObjectsCanvas(type); //保存している描画内容クリア
    } else {
        loadObjectsCanvas(type); //描画内容を読み込む
    }
    background_type = type; //背景タイプを記憶

    if (type == "teaching_material") {
        $("#tmview").css('height', $("#draw").height());
        document.getElementById("draw").style.display = "none";
        clearTimeout(id_timeout_setitemtype);
        selectItemType(ITEM_TYPE.SELECT);
        $("input[type=range]").fadeOut("fast");
        $(".captureall").fadeOut("fast");
    } else {
        document.getElementById("draw").style.display = "block";
        $("input[type=range]").fadeIn("fast");
        $(".captureall").fadeIn("fast");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//ホワイトボードモード(白背景/デフォルト背景)
///////////////////////////////////////////////////////////////////////////////////////////////////
function changeModeWhiteboard(type) { //type=whiteboard1/whiteboard2/whiteboard3/whiteboard4
    changeBackgroundBefore(type); //背景変更前処理
    if (type == "whiteboard1") {
        drawBackground(imgDefaultTmp1); //imgDefaultTmp1が空なら白背景
    } else if (type == "whiteboard2") {
        drawBackground(imgDefaultTmp2); //imgDefaultTmp2が空なら白背景
    } else if (type == "whiteboard3") {
        drawBackground(imgDefaultTmp3); //imgDefaultTmp3が空なら白背景
    } else if (type == "whiteboard4") {
        drawBackground(imgDefaultTmp4); //imgDefaultTmp4が空なら白背景
    } else {
        return false;
    }
    background_width = canvas.width;
    background_height = canvas.height;
    changeBackgroundAfter(type, false); //背景変更後処理
    return true;
}
function changeBackground(index, filepath) { //ホワイボードモードの背景変更(デフォルト背景設定)
    if (index < 1 && 4 < index) return;
    var img = null;
    if (filepath == null || filepath == "") {
        canvas.clear();
        canvas.backgroundColor = "rgba(255, 255, 255, 1.0)"; //白背景
        canvas.renderAll();
    } else {
        img = new Image();
        img.onload = function() {
            drawBackground(img); //キャンバスに画像をセット
        }
        img.src = filepath;
    }
    if (index == 1) {
        imgDefaultTmp1 = img;
    } else if (index == 2) {
        imgDefaultTmp2 = img;
    } else if (index == 3) {
        imgDefaultTmp3 = img;
    } else if (index == 4) {
        imgDefaultTmp4 = img;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//コンテンツモード(静止画/動画選択)
///////////////////////////////////////////////////////////////////////////////////////////////////
function clearFilePath(e) { this.value = null; }
window.addEventListener("DOMContentLoaded", function() {
    //コンテンツ選択
    selectContents.addEventListener("change", function(evt) {
        var img = null;
        var file = evt.target.files;
        var reader = new FileReader();
        //dataURL形式でファイルを読み込む
        var blob = file[0];
        reader.readAsDataURL(blob);
        //ファイルの読込が終了した時の処理
        reader.onload = function() {
            var file_type = blob.type;
            if (file_type.toLowerCase().indexOf("image") >= 0) {
                changeBackgroundBefore("image"); //背景変更前処理
                img = new Image();
                img.onload = function() {
                    drawBackground(img); //キャンバスに画像をセット
                }
                img.src = reader.result;
                imgBackgroundTmp = img;
                var modeList = document.getElementById("modeList");
                obj = document.modeList.selectedMode;
                for (var i = 0; i < modeList.length; i++) {
                    if (obj.options[i].value == "image") {
                        modeList.remove(i);
                        break;
                    }
                }
                $('#modeList').append('<option id="image" value="image" data-content="<img src=\'' + reader.result + '\' style=\'width:40px; height:30px; margin-top:5px; margin-bottom:5px;\'>&nbsp;<span style=\'display:inline-block;\'>' + TEXT_CONTENTS_IMAGE + '</span>"></option>'); //要素を追加
                $('select[name=selectedMode]').val("image");
                $('#modeList').selectpicker("refresh"); //リスト更新
                mode_current = "image";
                changeBackgroundAfter("image", true); //背景変更後処理
            } else if (file_type.toLowerCase().indexOf("video/mp4") >= 0 || file_type.toLowerCase().indexOf("video/webm") >= 0) {
                changeBackgroundBefore("video"); //背景変更前処理
                var video = document.querySelector('video');
                video.src = reader.result;
                video.onloadedmetadata = function(e) {
                    video.play(); //動画再生開始
                };
                videoBackgroundTmp = reader.result;
                var modeList = document.getElementById("modeList");
                obj = document.modeList.selectedMode;
                for (var i = 0; i < modeList.length; i++) {
                    if (obj.options[i].value == "video") {
                        modeList.remove(i);
                        break;
                    }
                }
                $('#modeList').append('<option id="video" value="video" data-content="<img src=\'./images/Movie.png\' style=\'width:40px; height:40px;\'>&nbsp;<span style=\'display:inline-block;\'>' + TEXT_CONTENTS_VIDEO + '</span>"></option>'); //要素を追加
                $('select[name=selectedMode]').val("video");
                $('#modeList').selectpicker("refresh"); //リスト更新
                mode_current = "video";
                changeBackgroundAfter("video", true); //背景変更後処理
            }
            $('.recording').fadeOut("fast");
            $(".rotation").fadeIn("fast");
            if (background_type == "video") {
                $(".freeze").fadeIn("fast");
            } else {
                $(".freeze").fadeOut("fast");
            }
            selectContents.value = ''; //次も選択できるようにクリア
        }
    }, false);
    //画像挿入ファイル選択
    selectInsertImage.addEventListener("change", function(evt) {
        var img = null;
        var file = evt.target.files;
        var reader = new FileReader();
        //dataURL形式でファイルを読み込む
        var blob = file[0];
        reader.readAsDataURL(blob);
        //ファイルの読込が終了した時の処理
        reader.onload = function() {
            var file_type = blob.type;
            if (file_type.toLowerCase().indexOf("image") >= 0) {
                img = new Image();
                img.onload = function() {
                    console.log("load inser image");
                }
                imgInsertPath = reader.result;
            }
        }
        selectInsertImage.value = ''; //次も選択できるようにクリア
    }, false);
    //アップロードファイル選択
//    selectUpload.addEventListener("change", function(evt) {
//        var img = null;
//        var file = evt.target.files;
//        var reader = new FileReader();
//        //dataURL形式でファイルを読み込む
//        var blob = file[0];
//        reader.readAsDataURL(blob);
//        //ファイルの読込が終了した時の処理
//        reader.onload = function() {
//            var file_type = blob.type;
//            var arraySplitBase64 = '';
//            if (typeof reader.result === 'string') {
//                // base64データのデータ本部を抽出
//                arraySplitBase64 = reader.result.split(',');
//                // file upload
//                console.log("[upload file]name : " + file[0].name + ", type : " + file[0].type);
//                uploadFile(googleDriveClient, arraySplitBase64[1], file[0].name, file[0].type);
//            } else {
//                throw 'read file error';
//            }
//        }
//        selectUpload.value = ''; //次も選択できるようにクリア
//    }, false);
});

// PC描画用
function changeModePcDraw(){
    imgBackgroundTmp = null
    mode_current = MODE.TRANSPARENT;
}

// 白背景表示用
function drawWiteBoard(){
    canvas.backgroundColor = "rgba(255, 255, 255, 1.0)"; //白背景
    canvas.renderAll();
    return;
}

function drawBackground(img) {
    if (img == null) {
        canvas.backgroundColor = "rgba(255, 255, 255, 1.0)"; //白背景
        return;
    }
    var canvasAspect = canvas.width / canvas.height, // canvasのアスペクト比
    imgAspect = img.width / img.height, // 画像のアスペクト比
    width, height;
    if(imgAspect >= canvasAspect) {
        width = canvas.width;
        height = canvas.width / imgAspect;
    } else {
        height = canvas.height;
        width = canvas.height * imgAspect;
    }
    background_width = width;
    background_height = height;

    setBackgroung(img.src, angle_rotation); //背景設定
}
function changeModeImage(filepath) {
    if (filepath == "" && imgBackgroundTmp != null) {
        changeBackgroundBefore("image"); //背景変更前処理
        drawBackground(imgBackgroundTmp);
        mode_current = MODE.IMAGE;
        changeBackgroundAfter("image", false); //背景変更後処理
    } else if (filepath != "") {
        changeBackgroundBefore("image"); //背景変更前処理
        var img = new Image();
        img.onload = function() {
            drawBackground(img); //キャンバスに画像をセット
        }
        img.src = filepath + "?t=" + new Date().getTime();
        imgBackgroundTmp = img;
        mode_current = MODE.IMAGE;
        changeBackgroundAfter("image", false); //背景変更後処理
    } else {
        console.log("changeModeImage error >> " + filepath);
        return false;
    }
    return true;
}
function changeModeTransparent(filepath) {
    if (filepath == "" && imgBackgroundTmp != null) {
        changeBackgroundBefore("transparent"); //背景変更前処理
        drawBackground(imgBackgroundTmp);
        mode_current = MODE.TRANSPARENT;
//        changeBackgroundAfter("transparent", false); //背景変更後処理
    } else if (filepath != "") {
        changeBackgroundBefore("transparent"); //背景変更前処理
        var img = new Image();
        img.onload = function() {
            drawBackground(img); //キャンバスに画像をセット
        }
        img.src = filepath + "?t=" + new Date().getTime();
        imgBackgroundTmp = img;
        mode_current = MODE.TRANSPARENT;
        background_type = "transparent"; //背景タイプを記憶
//        changeBackgroundAfter("transparent", false); //背景変更後処理
    } else {
        console.log("changeModeTransparent error >> " + filepath);
        return false;
    }
    return true;
}
function changeModeVideo(filepath) {
    if (filepath == "" && videoBackgroundTmp != null) {
        changeBackgroundBefore("video"); //背景変更前処理
        var video = document.querySelector('video');
        video.src = videoBackgroundTmp;
        video.onloadedmetadata = function(e) {
            video.play();
        };
        mode_current = MODE.VIDEO;
        changeBackgroundAfter("video", false); //背景変更後処理
    } else if (filepath != "") {
        changeBackgroundBefore("video"); //背景変更前処理
        var video = document.querySelector('video');
        video.src = filepath;
        video.onloadedmetadata = function(e) {
            video.play(); //動画再生開始
        };
        videoBackgroundTmp = reader.result;
        mode_current = MODE.VIDEO;
        changeBackgroundAfter("video", false); //背景変更後処理
    } else {
        console.log("changeModeVideo error >> " + filepath);
        return false;
    }
    return true;
}
function changeModeEducationalMaterial(filepath) {
    if (filepath != "" || (filepath == "" && tmBackgroundTmp != null)) {
        var set = "";
        if (filepath == "") {
            filepath = tmBackgroundTmp;
        }
        changeBackgroundBefore("teaching_material"); //背景変更前処理
        $('#tmview').remove();
        if (filepath.includes("https://drive.google.com/file/d/") && filepath.includes("/view?usp=sharing")) { //google driveで一般共有しているファイルを背景に設定
            set = '<iframe id="tmview" src="' + filepath.replace("/view?usp=sharing", "/preview") + '" width="100%" style="margin-top:-10px; margin-bottom:-10px; border: none;"></iframe>';
        } else if (filepath.includes("http")) { //インターネット上にあるPDFやPPTファイルを背景に設定
            set = '<iframe id="tmview" src="https://docs.google.com/viewer?url=' + filepath + '&embedded=true" width="100%" style="margin-top:-10px; margin-bottom:-10px; border: none;"></iframe>';
        } else if (filepath.includes(".pdf")) { //USE_LOCALがtrueの場合はローカルのファイルを背景に設定(PDFのみ)
            set = '<iframe id="tmview" src="' + filepath + '" width="100%" style="margin-top:-10px; margin-bottom:-10px; border: none;"></iframe>';
        }
        if (set != "") $('#live').append(set);
        tmBackgroundTmp = filepath;
        mode_current = MODE.EDUCATIONAL_MATERIAL;
        changeBackgroundAfter("teaching_material", false); //背景変更後処理
    } else {
        console.log("changeModeEducationalMaterial error >> " + filepath);
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//カメラモード
///////////////////////////////////////////////////////////////////////////////////////////////////
let localStream;
var camcount = 1;
var recorder = null;
var timer_facerocognition;
var list_reject_device = [];
var list_valid_device = [];
var map_device = {};
var is_init_checked = false;
var is_find_audio = false;
var camerror = 0;
function gotDevices(deviceInfos) {
    camcount = 1;   //used for labeling if the device label is not enumerated
    var modeList = document.getElementById("modeList");
    obj = document.modeList.selectedMode;
    for (var i = 0; i < modeList.length; i++) {
        if (!obj.options[i].value.includes("Whiteboard") &&
            obj.options[i].value != MODE.TRANSPARENT && //ブラウザ版では不要
            obj.options[i].value != MODE.CONTENTS && obj.options[i].value != MODE.IMAGE && obj.options[i].value != MODE.VIDEO && obj.options[i].value != MODE.EDUCATIONAL_MATERIAL) {
            modeList.remove(i);
            i--; //removeなので一個戻す
        }
    }
    $('#modeList').selectpicker("refresh"); //リスト更新
    //deviceid = null;
    var founddeviceid = false;
    var is_label_empty = false; //カメラ名取得失敗判定
    var is_change_cameramode = false; //カメラモード遷移判定
    for (var i = 0; i !== deviceInfos.length; ++i) {
        var deviceInfo = deviceInfos[i];
        if (list_reject_device.includes(deviceInfo.deviceId)) { //一度リトライしても接続できなかったデバイスについてはリストに追加しない
            continue;
        }
        var option = document.createElement('option');
        option.value = deviceInfo.deviceId;
        //console.log("deviceId::" + deviceInfo.deviceId);
        //console.log("groupId::" + deviceInfo.groupId);
        //console.log("kind::" + deviceInfo.kind);
        //console.log("label::" + deviceInfo.label);
        if (deviceInfo.kind === 'videoinput') {
            if (deviceid == null) {
                deviceid = deviceInfo.deviceId;
            } else if (deviceid == deviceInfo.deviceId) {
                founddeviceid = true;
            }
            if (camcount > 1) {
                option.text = deviceInfo.label || TEXT_MODE_CAMERA + ' ' + camcount;
            } else {
                option.text = deviceInfo.label || TEXT_MODE_CAMERA;
            }
            if (list_valid_device.length > 0) {
                list_valid_device.forEach(function(val, index, ar) {
                    var list = val.split("::"); //0:deviceId,1:label
                    if (deviceInfo.deviceId == list[0] && option.text.includes(TEXT_MODE_CAMERA)) {
                        option.text = list[1];
                    }
                });
            }
            var label = getCameraName(option.text); //カメラ名変換
            if (option.text != label) { //エルモカメラ接続時はカメラモードにする
                if (is_change_cameramode == false || deviceInfo.label.includes("(09a1:") == true) { //最初かエルモカメラの場合はdeviceidを変更
                    deviceid = deviceInfo.deviceId;
                }
                is_change_cameramode = true;
            }
            if (option.text != label || label.includes(TEXT_MODE_CAMERA)) { //カメラ名が取れなかった場合かカメラ名が取得できてELMOのPIDだった場合のみリストに追加
                option.text = label;
                $('#modeList').append('<option id="' + option.text + '" value="' + option.value + '" data-content="<img src=\'./images/CAMERA.png\' style=\'width:40px; height:40px;\'>&nbsp;<span style=\'display:inline-block;\'>' + option.text + '</span>"></option>'); //要素を追加
                $('#modeList').selectpicker("refresh"); //リスト更新
                camcount++;
                if (is_label_empty == false) {
                    is_label_empty = label.includes(TEXT_MODE_CAMERA) || list_valid_device.length == 0 ? true : false; //カメラ名取得失敗判定
                }
            }
        } else if (deviceInfo.kind === 'audioinput') {
            is_find_audio = true;
        }
    }
    if (!mode_current.includes("Whiteboard") &&
        mode_current != MODE.TRANSPARENT && //ブラウザ版では不要
        mode_current != MODE.IMAGE && mode_current != MODE.VIDEO && mode_current != MODE.EDUCATIONAL_MATERIAL && founddeviceid == false) {
        $('#wbmode').trigger('click');
        mode_current = MODE.WHITEBOARD1;
        $('.recording').fadeOut("fast");
        $(".rotation").fadeOut("fast");
        $(".freeze").fadeOut("fast");
    }

    //カメラモード中にリスト取得したときのために指定を変更しておく
    if (!mode_current.includes("Whiteboard") &&
        mode_current != MODE.TRANSPARENT && //ブラウザ版では不要
        mode_current != MODE.IMAGE && mode_current != MODE.VIDEO && mode_current != MODE.EDUCATIONAL_MATERIAL) {
        $('select[name=selectedMode]').val(mode_current);
        $('#modeList').selectpicker("refresh"); //リスト更新
    }

    //ローカルで使用する場合はカメラ名が取得できないので初回は一度接続に行く
    if (is_label_empty == true) {
        startVideo(0, 0, 0, false);
    }
    //ローカルでない場合は最初の1回のみエルモカメラ接続時はカメラモードにする
    else if (is_change_cameramode == true && is_init_checked == false) {
        $('select[name=selectedMode]').val(deviceid);
        $('#modeList').selectpicker("refresh"); //リスト更新
        changeModeCamera(deviceid);
    }
    is_init_checked = true;
}
function errorCallback(error) {
    console.log('navigator.getUserMedia error: ', error);
    camerror = -2;
}
//navigator.mediaDevices.enumerateDevices().then(gotDevices).catch(errorCallback);
// start local video
function startVideo(w, h, fr, retry) {
    if (localStream != null) {
        stopVideo();
    }
    navigator.mediaDevices = navigator.mediaDevices || ((navigator.mozGetUserMedia || navigator.webkitGetUserMedia) ? {
        getUserMedia: function(c) {
            return new Promise(function(y, n) {
                (navigator.mozGetUserMedia ||
                navigator.webkitGetUserMedia).call(navigator, c, y, n);
            });
        }
    } : null);
    if (!navigator.mediaDevices) {
        console.log("getUserMedia() not supported.");
        return;
    }
    if (w == 0 && h == 0 && retry == false) { //カメラ名取得処理中はなにもしない
    } else if(useBrowser == "safari") {
        navigator.mediaDevices.enumerateDevices().then(gotDevices).catch(errorCallback); //safariの場合にないと失敗する
    }
    var constraints = { audio: false, video: { width: { min: w }, height: { min: h } } }; //MX-1はaudioがfalseでないと映像でない
	is_find_audio = false;	// MXが機嫌悪くなるので音声は使わん
    if (useBrowser == "safari") {
        if (w == 0 && h == 0) {
            constraints = { audio: is_find_audio, video:{ deviceId: { exact: [deviceid] } }};
        } else {
            constraints = { audio: is_find_audio, video: { width: { min: w }, height: { min: h }, deviceId: { exact: [deviceid] } } };
        }
    } else {
        if (deviceid != null && w != 0 && h != 0) {
            constraints = { audio: is_find_audio, video: { width: w, height: h, frameRate: fr, deviceId: { exact: [deviceid] }} };
        } else if (w == 0 && h == 0) {
            constraints = { audio: is_find_audio, video:{ deviceId: deviceid }};
        } else {
            constraints = { audio: is_find_audio, video:{}};
            constraints.video.facingMode = {exact:"environment"}
        }
    }
    navigator.mediaDevices.getUserMedia(constraints).then(function(stream) {
        var videoTracks = stream.getVideoTracks();
        console.log('Using video device :: ' + videoTracks[0].label + ", resolution :: " + w + ", " + h);
        var modeList = document.getElementById("modeList");
        for (var i = 0; i < modeList.length; i++) {
            if (modeList.options[i].value == deviceid) {
                if (w == 0 && h == 0 && retry == false) {
                    var id = modeList.options[i].id;
                    var label = getCameraName(videoTracks[0].label);
                    $('#modeList').selectpicker("refresh"); //リスト更新
                    $('#modeList').splice(i, 1, '<option id="' + id + '" value="' + deviceid + '" data-content="<img src=\'./images/CAMERA.png\' style=\'width:40px; height:40px;\'>&nbsp;<span style=\'display:inline-block;\'>' + label + '</span>"></option>'); //要素を追加
                    $('#modeList').selectpicker("refresh"); //リスト更新
                    if (list_valid_device.indexOf(modeList.options[i].value + "::" + videoTracks[0].label) < 0) {
                        list_valid_device.push(modeList.options[i].value + "::" + videoTracks[0].label);
                        map_device[videoTracks[0].label.slice(-10, -1)] = modeList.options[i].value;
                    }
                } else {
                    if (useBrowser == "chrome" || useBrowser == "firefox") $('.recording').fadeIn("fast");
                    $(".rotation").fadeIn("fast");
                    $(".freeze").fadeIn("fast");
                    break;
                }
            }
            if (w == 0 && h == 0 && retry == false && modeList.options[i].id.includes(TEXT_MODE_CAMERA) == true && list_valid_device.indexOf(modeList.options[i].value + "::" + videoTracks[0].label) < 0) {
                var is_regged = false;
                list_valid_device.forEach(function(val, index, ar) {
                    var list = val.split("::"); //0:deviceId,1:label
                    if (modeList.options[i].value == list[0]) {
                        is_regged = true;
                    }
                });
                if (is_regged == false && modeList.options[i].value != deviceid) {
                    deviceid = modeList.options[i].value;
                    break;
                }
            }
        }
        localStream = stream;

        if (w == 0 && h == 0 && retry == false) {
            if (camcount - 1 != list_valid_device.length + list_reject_device.length) {
                if(!start_retry++){
                    startVideo(0, 0, 0, false);
                }
                return;
            } else if (camcount - 1 == list_valid_device.length + list_reject_device.length) {
                stopVideo();
                if (USE_LOCAL == false) {
                    is_init_checked = false;
                    navigator.mediaDevices.enumerateDevices().then(gotDevices).catch(errorCallback); //カメラ名取得は終わったようなので再度実行
                }
                return;
            }
        }

        var video = document.querySelector('video');
        video.srcObject = stream;
        video.onloadedmetadata = function(e) {
            video.play();
        };

        //顔認識開始
//        timer_facerocognition = setInterval("facerecognition()", 5000);
    },
    function(err) {
        console.log("The following error occurred: " + err.name + ":" + err.message);

//        if (retry == true && resolutions.length > r) {
//            startVideo(resolutions[r].width, resolutions[r].height, 3, retry);  // ぶん回しで繋いでも意味ないので消
//            r++;
//        } else {
            var modeList = document.getElementById("modeList");
            for (var i = 0; i < modeList.length; i++) {
                if (modeList.options[i].value == deviceid) {
                    modeList.remove(i); //削除
                    $('select[name=selectedMode]').val(MODE.WHITEBOARD1);
                    $('#modeList').selectpicker("refresh"); //リスト更新
//                    changeMode(); //ホワイトボードモードに遷移
//                    if (retry == true) alert(TEXT_CANNOT_USE_CAMERA);
                    list_reject_device.push(deviceid);
                    break;
                }
            }
			camerror = -1;
//        }
    });
}
// stop local video
function stopVideo() {
    //顔認識終了
//    clearInterval(timer_facerocognition);

    for (track of localStream.getTracks()) {
        track.stop();
        if (localStream.active) {
            console.log("stop " + track.kind + " : error");
        } else {
            console.log("stop " + track.kind + " : success");
        }
    }
    localStream = null;

    var video = document.getElementById('video');
    video.pause();
    video.src = '';
    video.srcObject = null;
}
function getCameraName(label) {
    if (label.includes("MO-1")) { //名前長いのでエルモのカメラのみ決めうちで表示する
        label = "MO-1/MO-1w";
    } else if (label.includes("MX-1") || label.includes("MX-P")) {
        label = "MX-1/MX-P";
    } else if (label.includes("TX-1") || label.includes("LX-1")) {
        label = "TX-1/LX-1";
    } else if (label.includes("L-12")) {
        label = "L-12i/L-12iD";
    } else if (label.includes("PX-30") || label.includes("PX-10")) {
        label = "PX-30/PX-10";
    } else if (label.includes("(09a1:")) { //未登録のELMOカメラ
        label = "ELMO camera";
    } else if (USE_LOCAL == false) {
        if (label.includes("facing front")) { //内蔵カメラ
            label = "Front-facing camera";
        } else if (label.includes("facing back")) { //内蔵カメラ
            label = "Back-facing camera";
        } else if (label.toLowerCase().includes("front") || label.toLowerCase().includes("facetime")) { //内蔵カメラ(前面)
            label = "Front-facing camera";
        } else if (label.toLowerCase().includes("back") || label.toLowerCase().includes("rear")) { //内蔵カメラ(背面)
            label = "Back-facing camera";
        } else if (label.toLowerCase().includes("web camera") || label.toLowerCase().includes("webcam")) { //内蔵カメラ
            label = "Web camera";
        } else {
            console.log(label);
//            if (!label.includes(TEXT_MODE_CAMERA)) label = "Web camera";
        }
    } else if (!label.includes(TEXT_MODE_CAMERA)) {
        label = "Web camera";
    }
    return label;
}
function isDocCam(label) {
    if (label == "MO-1/MO-1w" || label == "MX-1/MX-P" || label == "TX-1/LX-1" || label == "L-12i/L-12iD" || label == "PX-30/PX-10" || label == "ELMO camera") {
        return true;
    }
    return false;
}
//カメラ接続IF
function changeModeCamera(id) {
    if (USE_LOCAL == false) {
        deviceid = id;
    } else if (map_device[id] == undefined) {
        canvas.backgroundColor = "rgba(255, 255, 255, 1.0)"; //白背景
        $("#live").css('background-color', 'transparent'); //video透明背景
        document.getElementById("video").style.display = "none"; //video非表示
        return false;
    } else {
        deviceid = map_device[id];
    }
    changeBackgroundBefore("camera"); //背景変更前処理
    startVideo(camera_w, camera_h, camera_fr, true); //初期は720P
    mode_current = deviceid;
    changeBackgroundAfter("camera", false); //背景変更後処理
    start_retry = 0;
    return true;
}
//解像度設定IF
function setResolutionCamera(w, h, fr) {
    camera_w = w;
    camera_h = h;
    camera_fr = fr;
}
//リスト更新IF
function updateCameraList() {
    list_reject_device = [];
    list_valid_device = [];
    map_device = {};
    camerror = 0;
    navigator.mediaDevices.enumerateDevices().then(gotDevices).catch(errorCallback);
}
//エラー取得IF
function getError(){
    return camerror;
}
const resolutions = [
    {
        "label": "VGA",
        "width": 640,
        "height": 480,
        "ratio": "4:3"
    },
    {
        "label": "XGA",
        "width": 1024,
        "height": 768,
        "ratio": "4:3"
    },
    {
        "label": "QVGA",
        "width": 1280,
        "height": 960,
        "ratio": "4:3"
    },
    {
        "label": "720p(HD)",
        "width": 1280,
        "height": 720,
        "ratio": "16:9"
    },
    {
        "label": "WXGA",
        "width": 1280,
        "height": 800,
        "ratio": "16:10"
    },
    {
        "label": "SXGA",
        "width": 1280,
        "height": 1024,
        "ratio": "5:4"
    },
    {
        "label": "UXGA",
        "width": 1600,
        "height": 1200,
        "ratio": "4:3"
    },
    {
        "label": "1080p(FHD)",
        "width": 1920,
        "height": 1080,
        "ratio": "16:9"
    },
    {
        "label": "4K(UHD)",
        "width" : 3840,
        "height": 2160,
        "ratio": "16:9"
    },
    {
        "label": "3.0M",
        "width" : 2048,
        "height": 1536,
        "ratio": "4:3"
    },
    {
        "label": "13.0M",
        "width" : 4160,
        "height": 3120,
        "ratio": "4:3"
    }
];

///////////////////////////////////////////////////////////////////////////////////////////////////
//キャプチャ
///////////////////////////////////////////////////////////////////////////////////////////////////
var isVideoCaptureDownload = false;
var base64ToBlob = function(base64) {
    var base64Data = base64.split(',')[1], // Data URLからBase64のデータ部分のみを取得
        data = window.atob(base64Data), // base64形式の文字列をデコード
        buff = new ArrayBuffer(data.length),
        arr = new Uint8Array(buff),
        blob, i, dataLen;
    // blobの生成
    for( i = 0, dataLen = data.length; i < dataLen; i++) {
        arr[i] = data.charCodeAt(i);
    }
    blob = new Blob([arr], {type: 'image/png'});
    return blob;
}
function getImageVideo() {
    var video = document.getElementById("video");
    video.pause(); //停止

    var canvasAspect = canvas.width / canvas.height, // canvasのアスペクト比
    imgAspect = video.videoWidth / video.videoHeight, // ビデオのアスペクト比
    width, height;
    var canvasTmp = document.createElement("canvas");
    var ctxTmp = canvasTmp.getContext('2d');
    canvasTmp.setAttribute('width',canvas.width);
    canvasTmp.setAttribute('height',canvas.height);
    ctxTmp.save();
    ctxTmp.fillStyle = "black";
    ctxTmp.fillRect(0, 0, canvas.width, canvas.height);
    ctxTmp.translate(parseInt(canvas.width / 2), parseInt(canvas.height / 2));
    if (angle_rotation == 180) {
        ctxTmp.rotate(angle_rotation * Math.PI / 180);
    }
    if(imgAspect >= canvasAspect) {
        width = canvas.width;
        height = canvas.width / imgAspect;
    } else {
        height = canvas.height;
        width = canvas.height * imgAspect;
    }
    ctxTmp.drawImage(video, 0, 0, video.videoWidth, video.videoHeight, parseInt(-(width/2)), parseInt(-(height/2)), width, height);
    ctxTmp.restore();

    var img = new Image();
    img.onload = function() { }
    img.src = canvasTmp.toDataURL("image/png");
    imgLiveTmp = img;

    if (is_freeze == false) {
        video.play(); //再開
    }
}
function download_image(posx, posy, width, height) {
    $(".statuscap").fadeIn("fast");

    //時刻取得
    var now = new Date();
    var y = now.getFullYear();
    var m = now.getMonth() + 1;
    var d = now.getDate();
    var h = now.getHours();
    var mi = now.getMinutes();
    var s = now.getSeconds();
    var mm = ('0' + m).slice(-2);
    var dd = ('0' + d).slice(-2);
    var hh = ('0' + h).slice(-2);
    var mmi = ('0' + mi).slice(-2);
    var ss = ('0' + s).slice(-2);

    //前処理
    if (background_type == "camera" || background_type == "video") {
        isVideoCaptureDownload = true;
        if (imgLiveTmp == null) {
            getImageVideo();
            setTimeout(function() { download_image(posx, posy, width, height) }, 100);
            return;
        }
    }

    //ダウンロード用canvas作成
    var canvasDownload = document.createElement("canvas");
    var ctxDownload = canvasDownload.getContext('2d');
    if (width == 0 || height == 0) {
        width = can.width;
        height = can.height;
    }
    canvasDownload.setAttribute('width',width);
    canvasDownload.setAttribute('height',height);
    ctxDownload.save();

    //画像生成
    //html2canvasでのズーム状態のキャプチャはできる場合とできない場合があるのでひとまずコメントアウト
/*    if (scale_zoom != 1.0) {
        html2canvas($("#background"), {
            onrendered: function (canvas_b) {
                html2canvas($("#draw"), {
                    onrendered: function (canvas_d) {
                        ctxDownload.drawImage(canvas_b, 0, 0, width, height);
                        ctxDownload.drawImage(canvas_d, 0, 0, width, height);
                        ctxDownload.restore();
                        var blob = base64ToBlob(canvasDownload.toDataURL("image/png"));
                        var link = document.createElement( 'a' );
                        link.href = window.URL.createObjectURL( blob );
                        link.download = "IMAG" + y + mm + dd + hh + mmi + ss + ".png";
                        link.click();
                    }
                });
            }
        });
    } else */
    {
        if (isVideoCaptureDownload == true && imgLiveTmp != null) {
            ctxDownload.drawImage(imgLiveTmp, posx, posy, width, height, 0, 0, width, height); //描画内容
        }
        ctxDownload.drawImage(can, posx, posy, width, height, 0, 0, width, height); //描画内容
        ctxDownload.restore();

        //ダウンロード
        var blob = base64ToBlob(canvasDownload.toDataURL());
        var link = document.createElement( 'a' );
        link.href = window.URL.createObjectURL( blob );
        link.download = "IMAG" + y + mm + dd + hh + mmi + ss + ".png";
        link.click();
    }

    //後処理
    if (isVideoCaptureDownload == true) {
        imgLiveTmp = null;
        setTimeout(function() { canvas_resize(); }, 1000); //リサイズ処理してその先でフリーズ解除させる
    }
    setTimeout(function() { $(".statuscap").fadeOut("fast"); }, timeout_status);
}
function selectAreaCapture() {
    if (background_type == "camera" || background_type == "video") { //カメラ/動画
        freeze(); //一時停止
    }
    selectItemType(ITEM_TYPE.RECT_SELECT);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//録画
///////////////////////////////////////////////////////////////////////////////////////////////////
// 録画開始
var start_rec = false;
var cnt_rec = 0;
//Whammyでの静止画背景+線の録画は端末によってキャプチャタイミングで線の描画が飛ぶのでひとまずコメントアウト
/* function createContext() {
    var canvasTmp = document.createElement("canvas");
    var ctxTmp = canvasTmp.getContext('2d');
    canvasTmp.setAttribute('width',can.width);
    canvasTmp.setAttribute('height',can.height);
    ctxTmp.save();
    ctxTmp.fillStyle = "rgb(0, 0, 0)";
    ctxTmp.fillRect(0, 0, can.width, can.height);
    ctxTmp.drawImage(can, 0, 0, can.width, can.height); //描画内容
    ctxTmp.restore();
    return ctxTmp;
}
function nextFrame() {
    if (start_rec == true) {
        cnt_rec++;
        if (cnt_rec == 6) { //requestAnimationFrameが毎秒60回,10フレームの録画なので6回に1回保存
            cnt_rec = 0;
            var context = createContext();
            recorder.add(context);
        }
        requestAnimationFrame(nextFrame);
    }
} */
function startRecording() {
    if (localStream != null) { //ライブ録画
        recorder = new MediaRecorder(localStream);
        recorder.ondataavailable = function(evt) {
        }
        recorder.start();
        $(".statusrec").fadeIn("fast");
    } else { //静止画背景録画
/*        recorder = new Whammy.Video(10);

        //録画中フラグ更新
        start_rec = true;
        cnt_rec = 0;

        //録画開始
        var context = createContext();
        recorder.add(context);
        requestAnimationFrame(nextFrame); */
        return;
    }
    is_recording = true;
    $(".recording").attr('src','images/Rec_S.png');
}
// 録画停止
function stopRecording() {
    if (localStream != null) { //ライブ録画
        recorder.stop();
        download_video();
        $(".statusrec").fadeOut("fast");
    } else { //静止画背景録画
/*        //録画中フラグ更新
        start_rec = false;

        //動画生成
        var output = recorder.compile();

        //時刻取得
        var now = new Date();
        var y = now.getFullYear();
        var m = now.getMonth() + 1;
        var d = now.getDate();
        var h = now.getHours();
        var mi = now.getMinutes();
        var s = now.getSeconds();
        var mm = ('0' + m).slice(-2);
        var dd = ('0' + d).slice(-2);
        var hh = ('0' + h).slice(-2);
        var mmi = ('0' + mi).slice(-2);
        var ss = ('0' + s).slice(-2);

        //ダウンロード
        var link = document.createElement( 'a' );
        link.href = window.URL.createObjectURL( output );
        link.download = "MOV_" + y + mm + dd + hh + mmi + ss + ".webm";
        link.click(); */
        return;
    }
    is_recording = false;
    $(".recording").attr('src','images/Rec_N.png');
}
function download_video() {
    recorder.ondataavailable = function(e) {
        var blob = new Blob([e.data], { type: e.data.type });
        blobUrl = window.URL.createObjectURL(blob);

        //時刻取得
        var now = new Date();
        var y = now.getFullYear();
        var m = now.getMonth() + 1;
        var d = now.getDate();
        var h = now.getHours();
        var mi = now.getMinutes();
        var s = now.getSeconds();
        var mm = ('0' + m).slice(-2);
        var dd = ('0' + d).slice(-2);
        var hh = ('0' + h).slice(-2);
        var mmi = ('0' + mi).slice(-2);
        var ss = ('0' + s).slice(-2);

        //ダウンロード
        var link = document.createElement( 'a' );
        link.href = window.URL.createObjectURL( blob );
        link.download = "MOV_" + y + mm + dd + hh + mmi + ss + ".webm";
        link.click();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//フリーズ
///////////////////////////////////////////////////////////////////////////////////////////////////
function freeze() {
    if (is_freeze == false) { //カメラライブ停止
        $(".freeze").attr('src','images/Freeze_S.png');
        is_freeze = true;

        var video = document.getElementById("video");
        video.pause(); //停止

        var canvasAspect = canvas.width / canvas.height, // canvasのアスペクト比
        imgAspect = video.videoWidth / video.videoHeight, // ビデオのアスペクト比
        width, height;
        if(imgAspect >= canvasAspect) {
            width = canvas.width;
            height = canvas.width / imgAspect;
        } else {
            height = canvas.height;
            width = canvas.height * imgAspect;
        }
        background_width = width;
        background_height = height;

//        $(".statusfreeze").fadeIn("fast");
    } else if (is_freeze == true) { //カメラライブ再開
        $(".freeze").attr('src','images/Freeze_N.png');
        is_freeze = false;
        var video = document.getElementById("video");
        video.play(); //再開
//        $(".statusfreeze").fadeOut("fast");
    }
    adjustPosZoom(); //ズームしていた場合は位置調整
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//回転
///////////////////////////////////////////////////////////////////////////////////////////////////
function rotation(draw) {
    if (angle_rotation == 0) {
        $(".rotation").attr('src','images/Rotation_S.png');
        angle_rotation = 180;
    } else {
        $(".rotation").attr('src','images/Rotation_N.png');
        angle_rotation = 0;
    }
    document.getElementById("video").style.transform = "scale(" + scale_zoom + ") rotate(" + angle_rotation + "deg)";
    if (draw == true) {
        if (background_type == "whiteboard1") {
            drawBackground(imgDefaultTmp1);
        } else if (background_type == "whiteboard2") {
            drawBackground(imgDefaultTmp2);
        } else if (background_type == "whiteboard3") {
            drawBackground(imgDefaultTmp3);
        } else if (background_type == "whiteboard4") {
            drawBackground(imgDefaultTmp4);
        } else if (background_type == "image") {
            drawBackground(imgBackgroundTmp);
        }
    }
    adjustPosZoom(); //ズームしていた場合は位置調整
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//画像挿入
///////////////////////////////////////////////////////////////////////////////////////////////////
function insertimage(path) {
    selectItemType(ITEM_TYPE.IMAGE);
    if (path == "") {
        setTimeout(function(){ $(selectInsertImage).click(); },200);
    } else {
        imgInsertPath = path;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//拡大
///////////////////////////////////////////////////////////////////////////////////////////////////
var item_type_tmp = ITEM_TYPE.LINE_FREE_PEN;
function changeZoom () {
    adjustPosZoom(); //ズームしていた場合は位置調整
    scale_zoom = $("input[type=range]").val(); //ズーム倍率保存
    zoom(); //ズーム処理

//    if (background_type == "camera" || background_type == "video") { //カメラ/動画
        if (scale_zoom != 1.0) {
            hideSettingPen();
            hideSettingMarker();
            hideSettingEraser();
            $('.pencil').fadeOut("fast");
            $('.marker').fadeOut("fast");
            $('.clearObject').fadeOut("fast");
        } else {
            $('.pencil').fadeIn("fast");
            $('.marker').fadeIn("fast");
            $('.clearObject').fadeIn("fast");
        }
//ズーム中の描画検討
//    } else { //ホワイトボード/静止画
//        if (scale_zoom != 1.0) {
//            $('.selectitem').fadeIn("fast");
//        } else {
//            $('.selectitem').fadeOut("fast");
//        }
//    }
}
function zoom() {
    if (background_type == "camera" || background_type == "video") { //カメラ/動画
        document.getElementById("video").style.transform = "scale(" + scale_zoom + ") rotate(" + angle_rotation + "deg)";
//ズーム中の描画検討
        if (scale_zoom != 1.0 && item_type != ITEM_TYPE.ZOOM) {
            item_type_tmp = item_type;
        } else if (scale_zoom != 1.0 && item_type == ITEM_TYPE.ZOOM) {
        } else {
            is_show_setting_pen = true; //サブメニューでるとうざいので表示しないためにフラグを変更
            $(".pencil").trigger('click'); //ズーム解除したら気を利かせてペンにする
        }
        canvas.zoomToPoint(new fabric.Point(($("#draw").width()) / 2, ($("#draw").height()) / 2), scale_zoom);
        canvas.setWidth(Math.ceil($("#draw").width() * scale_zoom));
        canvas.setHeight(Math.ceil($("#draw").height() * scale_zoom));
        canvas.renderAll();
//ズーム中の描画検討

//        if (scale_zoom != 1.0 && item_type != ITEM_TYPE.ZOOM) {
//            item_type_tmp = item_type;
//            selectItemType(ITEM_TYPE.ZOOM);
//        } else if (scale_zoom != 1.0 && item_type == ITEM_TYPE.ZOOM) {
//            selectItemType(ITEM_TYPE.ZOOM);
//        } else {
//            clearTimeout(id_timeout_setitemtype);
//            id_timeout_setitemtype = setTimeout(function(){ selectItemType(item_type_tmp) }, 200); //少しおいてからカーソルを戻す
//        }
//        document.getElementById("canvas").style.transform = "scale(" + scale_zoom + ")";
    } else { //ホワイトボード/静止画
//ズーム中の描画検討
        if (scale_zoom != 1.0 && item_type != ITEM_TYPE.ZOOM) {
            item_type_tmp = item_type;
        } else if (scale_zoom != 1.0 && item_type == ITEM_TYPE.ZOOM) {
        } else {
            is_show_setting_pen = true; //サブメニューでるとうざいので表示しないためにフラグを変更
            $(".pencil").trigger('click'); //ズーム解除したら気を利かせてペンにする
        }
        canvas.zoomToPoint(new fabric.Point(($("#draw").width()) / 2, ($("#draw").height()) / 2), scale_zoom);
        canvas.setWidth(Math.ceil($("#draw").width() * scale_zoom));
        canvas.setHeight(Math.ceil($("#draw").height() * scale_zoom));
        canvas.renderAll();
//ズーム中の描画検討

//        if (scale_zoom != 1.0 && item_type != ITEM_TYPE.ZOOM) {
//            item_type_tmp = item_type;
//            selectItemType(ITEM_TYPE.ZOOM);
//        } else if (scale_zoom != 1.0 && item_type == ITEM_TYPE.ZOOM) {
//            selectItemType(ITEM_TYPE.ZOOM);
//        } else {
//            clearTimeout(id_timeout_setitemtype);
//            id_timeout_setitemtype = setTimeout(function(){ selectItemType(item_type_tmp) }, 200); //少しおいてからカーソルを戻す
//        }
//        document.getElementById("canvas").style.transform = "scale(" + scale_zoom + ")";
    }
}
function adjustPosZoom () {
    if (background_type == "camera" || background_type == "video") { //カメラ/動画
        var video = document.querySelector('video');
        var canvasAspect = canvas.width / canvas.height, // canvasのアスペクト比
        imgAspect = video.videoWidth / video.videoHeight, // ビデオのアスペクト比
        width, height;
        if(imgAspect >= canvasAspect) {
            width = canvas.width;
            height = canvas.width / imgAspect;
        } else {
            height = canvas.height;
            width = canvas.height * imgAspect;
        }
        background_width = width;
        background_height = height;

//ズーム中の描画検討
        canvas.relativePan(new fabric.Point(-1 * moveX_draw, -1 * moveY_draw));
        moveX_draw = 0; moveY_draw = 0;
        if ($("input[type=range]").val() != 1.0) {
            /* 何もしない */
        } else {
            moveX = 0; moveY = 0;
        }
//ズーム中の描画検討

        rangeX = Math.ceil((background_width * $("input[type=range]").val() - canvas.width) / 2);
        rangeY = Math.ceil((background_height * $("input[type=range]").val() - canvas.height) / 2);

        moveX = Math.ceil((moveX * $("input[type=range]").val()) / scale_zoom);
        moveY = Math.ceil((moveY * $("input[type=range]").val()) / scale_zoom);

        if (-1 * rangeX > moveX) moveX = -1 * rangeX;
        if (rangeX < moveX) moveX = rangeX;
        if (-1 * rangeY > moveY) moveY = -1 * rangeY;
        if (rangeY < moveY) moveY = rangeY;

        if ($("input[type=range]").val() != 1.0) {
            if (-1 * rangeX <= moveX && rangeX >= moveX) {
                $('#video').css('margin-left', moveX + 'px');
                $('#canvas').css('margin-left', moveX + 'px');
            } else {
                $('#video').css('margin-left', '0px');
                $('#canvas').css('margin-left', '0px');
            }
            if (-1 * rangeY <= moveY && rangeY >= moveY) {
                $('#video').css('margin-top', moveY + 'px');
                $('#canvas').css('margin-top', moveY + 'px');
            } else {
                $('#video').css('margin-top', '0px');
                $('#canvas').css('margin-top', '0px');
            }
        } else {
            moveX = 0; moveY = 0;
            $('#video').css('margin-left', '0px');
            $('#video').css('margin-top', '0px');
            $('#canvas').css('margin-left', '0px');
            $('#canvas').css('margin-top', '0px');
        }
    } else { //ホワイトボード/静止画
//ズーム中の描画検討
        canvas.relativePan(new fabric.Point(-1 * moveX_draw, -1 * moveY_draw));
        moveX_draw = 0; moveY_draw = 0;
        if ($("input[type=range]").val() != 1.0) {
            /* 何もしない */
        } else {
            moveX = 0; moveY = 0;
        }
//ズーム中の描画検討

        rangeX = Math.ceil((background_width * $("input[type=range]").val() - canvas.width) / 2);
        rangeY = Math.ceil((background_height * $("input[type=range]").val() - canvas.height) / 2);
        moveX = Math.ceil((moveX * $("input[type=range]").val()) / scale_zoom);
        moveY = Math.ceil((moveY * $("input[type=range]").val()) / scale_zoom);
        if (-1 * rangeX > moveX) moveX = -1 * rangeX;
        if (rangeX < moveX) moveX = rangeX;
        if (-1 * rangeY > moveY) moveY = -1 * rangeY;
        if (rangeY < moveY) moveY = rangeY;
        if ($("input[type=range]").val() != 1.0) {
            if (-1 * rangeX <= moveX && rangeX >= moveX) {
                $('#canvas').css('margin-left', moveX + 'px');
            } else {
                $('#canvas').css('margin-left', '0px');
            }
            if (-1 * rangeY <= moveY && rangeY >= moveY) {
                $('#canvas').css('margin-top', moveY + 'px');
            } else {
                $('#canvas').css('margin-top', '0px');
            }
        } else {
            moveX = 0; moveY = 0;
            $('#canvas').css('margin-left', '0px');
            $('#canvas').css('margin-top', '0px');
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//ボタン選択状態変更
///////////////////////////////////////////////////////////////////////////////////////////////////
function selectItemType(type) {
    if (type.includes(ITEM_TYPE.LINE_FREE_PEN)) {
        changePenIcon("S");
        changeMarkerIcon("N");
        $(".clearObject").attr('src','images/Eraser_part_N.png');
        $(".selectitem").attr('src','images/Hand_N.png');
    } else if (type.includes(ITEM_TYPE.LINE_FREE_MARKER)) {
        changePenIcon("N");
        changeMarkerIcon("S");
        $(".clearObject").attr('src','images/Eraser_part_N.png');
        $(".selectitem").attr('src','images/Hand_N.png');
    } else if (type == ITEM_TYPE.ERASER) {
        changePenIcon("N");
        changeMarkerIcon("N");
        $(".clearObject").attr('src','images/Eraser_part_S.png');
        $(".selectitem").attr('src','images/Hand_N.png');
    } else if (type == ITEM_TYPE.SELECT || type == ITEM_TYPE.ZOOM) {
        changePenIcon("N");
        changeMarkerIcon("N");
        $(".clearObject").attr('src','images/Eraser_part_N.png');
        $(".selectitem").attr('src','images/Hand_S.png');
    }
    setItemType(type);
}
function changePenIcon(state) {
    if (color_pen.slice(0,-4) == "rgba(0,0,0,") {
        $(".pencil").attr('src','images/PEN_Black_' + state + '.png');
    } else if (color_pen.slice(0,-4) == "rgba(255,0,0,") {
        $(".pencil").attr('src','images/PEN_Red_' + state + '.png');
    } else if (color_pen.slice(0,-4) == "rgba(0,255,0,") {
        $(".pencil").attr('src','images/PEN_Green_' + state + '.png');
    } else if (color_pen.slice(0,-4) == "rgba(0,0,255,") {
        $(".pencil").attr('src','images/PEN_Blue_' + state + '.png');
    } else if (color_pen.slice(0,-4) == "rgba(255,255,0,") {
        $(".pencil").attr('src','images/PEN_Yellow_' + state + '.png');
    } else if (color_pen.slice(0,-4) == "rgba(255,0,255,") {
        $(".pencil").attr('src','images/PEN_Magenta_' + state + '.png');
    } else if (color_pen.slice(0,-4) == "rgba(0,255,255,") {
        $(".pencil").attr('src','images/PEN_Cyan_' + state + '.png');
    } else if (color_pen.slice(0,-4) == "rgba(255,255,255,") {
        $(".pencil").attr('src','images/PEN_White_' + state + '.png');
    }
}
function changeMarkerIcon(state) {
    if (color_marker.slice(0,-4) == "rgba(0,0,0,") {
        $(".marker").attr('src','images/MARKER_Black_' + state + '.png');
    } else if (color_marker.slice(0,-4) == "rgba(255,0,0,") {
        $(".marker").attr('src','images/MARKER_Red_' + state + '.png');
    } else if (color_marker.slice(0,-4) == "rgba(0,255,0,") {
        $(".marker").attr('src','images/MARKER_Green_' + state + '.png');
    } else if (color_marker.slice(0,-4) == "rgba(0,0,255,") {
        $(".marker").attr('src','images/MARKER_Blue_' + state + '.png');
    } else if (color_marker.slice(0,-4) == "rgba(255,255,0,") {
        $(".marker").attr('src','images/MARKER_Yellow_' + state + '.png');
    } else if (color_marker.slice(0,-4) == "rgba(255,0,255,") {
        $(".marker").attr('src','images/MARKER_Magenta_' + state + '.png');
    } else if (color_marker.slice(0,-4) == "rgba(0,255,255,") {
        $(".marker").attr('src','images/MARKER_Cyan_' + state + '.png');
    } else if (color_marker.slice(0,-4) == "rgba(255,255,255,") {
        $(".marker").attr('src','images/MARKER_White_' + state + '.png');
    }
}
