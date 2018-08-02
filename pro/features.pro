TEMPLATE = subdirs
CONFIG *= ordered

HEADERS *= \
    $$files( $${PWD}/../include/*.h ) \
    $$files( $${PWD}/../include/*.hpp ) \

OTHER_FILES *= \
    $$files( $${PWD}/../*.prf ) \
    $$files( $${PWD}/../doc/ru/*.md ) \
