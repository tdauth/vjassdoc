
#include(FindLibintl)
#gettext_create_translations(de/LC_MESSAGES/vjassdoc.mo de/LC_MESSAGES/vjassdoc.po)
#gettext_create_translations(en/LC_MESSAGES/vjassdoc.mo en/LC_MESSAGES/vjassdoc.po)
#gettext_create_translations(mk/LC_MESSAGES/vjassdoc.mo mk/LC_MESSAGES/vjassdoc.po)


# install

install(FILES
de/LC_MESSAGES/vjassdoc.mo
DESTINATION share/locale/de/LC_MESSAGES/
)
install(FILES
en/LC_MESSAGES/vjassdoc.mo
DESTINATION share/locale/en/LC_MESSAGES/
)
install(FILES
mk/LC_MESSAGES/vjassdoc.mo
DESTINATION share/locale/mk/LC_MESSAGES/
)