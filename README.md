# cpp-simple-vector
Упрощённая реализация вектора подобному стандартной библиотеки.
Поддерживает следующий функционал:
- иницилизаця(размером N элементов инициализированных значением по умолчанию, из одинаковых значений размером N, из std::initializer_list);
- копирование и перемещение;
- резервирование N элементов, изменение размера;
- получение размера вектора и вместимости;
- обращение к элементам по индексу
- добавление нового элемента в конец вектора (push_back) или в произвольное место (insert);
- удаление последнего элемента (pop_back) или из проивольного места;
- итераторы;
- очисту и обмен с другим вектором.
Использует вспомогательный файл array_ptr.h, реализующий упрощённое взаимодействие с массивом указателей.
Используется стандарт C++17.