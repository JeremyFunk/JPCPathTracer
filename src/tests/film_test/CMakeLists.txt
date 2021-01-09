set(AUTOSRC
"basic_film.cpp"
)
set(INC
"../")

set(LIBS
    core
    films
    filters
)

jpctr_add_gtest(film_test "${AUTOSRC}" "${INC}" "${LIBS}" )