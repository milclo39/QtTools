#ifndef INPUTUNICODE_H
#define INPUTUNICODE_H
#include <QTextCodec>

//Unicode入力用関数
//※vcコンパイルの場合?ファイルをutf-8(BOM有)で保存する必要あり
#define inputUnicode(dataText) QTextCodec::codecForLocale()->toUnicode(dataText)

#endif //INPUTUNICODE_H
