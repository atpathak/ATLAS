rm -rf InstallArea WorkArea
setupWorkArea.py
cd WorkArea/cmt ; cmt config ; cmt bro gmake clean ; cmt bro gmake -j8 ; cd -
