QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
RC_ICONS = icon.ico
DESTDIR = bin

SOURCES += \
    adduseritem.cpp \
    applyfriend.cpp \
    bubbleframe.cpp \
    cachemanager.cpp \
    chatdialog.cpp \
    chatitembase.cpp \
    chatpage.cpp \
    chatuserlist.cpp \
    chatuserwid.cpp \
    chatview.cpp \
    clickbutton.cpp \
    clickedoncelabel.cpp \
    clicklabel.cpp \
    clicklabelplus.cpp \
    contactinfopage.cpp \
    contacttreewidget.cpp \
    customitemdelegate.cpp \
    customizeedit.cpp \
    customscrollarea.cpp \
    eyebutton.cpp \
    filebubble.cpp \
    fileclient.cpp \
    filetransferwidget.cpp \
    forgetpwddialog.cpp \
    friendlabel.cpp \
    global.cpp \
    globalsearchpage.cpp \
    gvcbutton.cpp \
    httpmgr.cpp \
    imageviewer.cpp \
    listitembase.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    messagetextedit.cpp \
    newfriendinfopage.cpp \
    picturebubble.cpp \
    progressbar.cpp \
    registedialog.cpp \
    searchlist.cpp \
    searchuserfrienditem.cpp \
    searchuseritem.cpp \
    searchuserlist.cpp \
    searchusernofrienditem.cpp \
    sqlmgr.cpp \
    statewidget.cpp \
    tcpmgr.cpp \
    textbubble.cpp \
    userdata.cpp \
    usermgr.cpp \
    utils.cpp \
    workthread.cpp

HEADERS += \
    Semaphore.h \
    adduseritem.h \
    applyfriend.h \
    bubbleframe.h \
    cachemanager.h \
    chatdialog.h \
    chatitembase.h \
    chatpage.h \
    chatuserlist.h \
    chatuserwid.h \
    chatview.h \
    clickbutton.h \
    clickedoncelabel.h \
    clicklabel.h \
    clicklabelplus.h \
    contactinfopage.h \
    contacttreewidget.h \
    customitemdelegate.h \
    customizeedit.h \
    customscrollarea.h \
    eyebutton.h \
    filebubble.h \
    fileclient.h \
    filetransferwidget.h \
    forgetpwddialog.h \
    friendlabel.h \
    global.h \
    globalsearchpage.h \
    gvcbutton.h \
    httpmgr.h \
    imageviewer.h \
    listitembase.h \
    loadingdlg.h \
    logindialog.h \
    mainwindow.h \
    messagetextedit.h \
    newfriendinfopage.h \
    picturebubble.h \
    progressbar.h \
    registedialog.h \
    searchlist.h \
    searchuserfrienditem.h \
    searchuseritem.h \
    searchuserlist.h \
    searchusernofrienditem.h \
    singleton.h \
    sqlmgr.h \
    statewidget.h \
    tcpmgr.h \
    textbubble.h \
    userdata.h \
    usermgr.h \
    utils.h \
    workthread.h

FORMS += \
    adduseritem.ui \
    applyfriend.ui \
    chatdialog.ui \
    chatpage.ui \
    chatuserwid.ui \
    contactinfopage.ui \
    forgetpwddialog.ui \
    friendlabel.ui \
    globalsearchpage.ui \
    loadingdlg.ui \
    logindialog.ui \
    mainwindow.ui \
    newfriendinfopage.ui \
    registedialog.ui \
    searchuserfrienditem.ui \
    searchuseritem.ui \
    searchusernofrienditem.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    config.ini \
    config.ini

unix:CONFIG(debug, debug | release){
    TargetConfig = $${PWD}/config.ini
    OutputDir =  $${OUT_PWD}/$${DESTDIR}
    StaticDir = $${PWD}/static

    QMAKE_POST_LINK += cp -r \"$$TargetConfig\" \"$$OutputDir/\" && mkdir -p \"$$OutputDir/static/\" && cp -r \"$$StaticDir\"/* \"$$OutputDir/static/\"
}else{
    TargetConfig = $${PWD}/config.ini
    OutputDir =  $${OUT_PWD}/$${DESTDIR}
    StaticDir = $${PWD}/static

    QMAKE_POST_LINK += cp -r \"$$TargetConfig\" \"$$OutputDir/\" && mkdir -p \"$$OutputDir/static/\" && cp -r \"$$StaticDir\"/* \"$$OutputDir/static/\"
}
