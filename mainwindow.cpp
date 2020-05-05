#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    window->setLayout( layout );
    this->setCentralWidget( window );
    this->setStyleSheet( "QWidget { background-color: white; }" );
    generateWindow_Input();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//ПОСТРОЕНИЕ ОКНА
//Генерация окна входа
void MainWindow::generateWindow_Input()
{

    this->setWindowTitle("Вход");

    QTextEdit *text_welcom = new QTextEdit();
    text_welcom->setReadOnly( true );

    QTextEdit *text_fio = new QTextEdit;
    //text_fio->setPlaceholderText( "Введите ФИО..." );

    QPushButton* pushButtom_continue = new QPushButton( tr("Продолжить") );
    connect(pushButtom_continue, SIGNAL(clicked()), this, SLOT( on_pushButton_continue_clicked()));

    layout->addWidget( text_welcom );
    layout->addWidget( text_fio );
    layout->addWidget( pushButtom_continue );


    flag_input_window = true;
    QTextEdit *te =  qobject_cast<QTextEdit *>( layout->itemAt(1)->widget() );
    te->installEventFilter( this );

    //text_fio->setMaximumHeight( 25 );
    //Построение мапы вопросов
    Fill_Map_Topic();
}


//Функция заполнения мапы с названиями тем и количеством вопросов
//Сделано для облегчения поиска файлов( хранить несколько названий тем и кол-ва вопросов менее затратно, чем хранить 500строк адресов )
void MainWindow::Fill_Map_Topic()
{
    int current_all_question = 0;
    QDir dir_main; //Прописываем главный путь откуда искать( У нас папка "Вопросы" лежит в билде )

    dir_main.setPath( "Вопросы" );

    QStringList namesOfDirectories_in_main = dir_main.entryList(); //Получаем список тем в папке "Вопросы"
    //namesOfDirectories_in_main.removeFirst();
    //namesOfDirectories_in_main.removeFirst(); // Удаляем первые два елемента "."  и ".."

    //Идем по темам, открываем все вопросы и проверяем их на валидность, а так же считаем общее число вопросов
     for ( int i=0; i < namesOfDirectories_in_main.size(); ++i )
     {
         //если не находим точек в названии файла, т.е. это не расширение, то значит это потенциальная папка с темой, значит заходим
        if( namesOfDirectories_in_main[ i ].indexOf( "." ) == - 1 )
        {
            QString name_directory = namesOfDirectories_in_main[ i ]; //сохраняем название темы, перед входом в папку

            dir_main.cd( namesOfDirectories_in_main[ i ] ); //вход в директорию
            QStringList namesOfQuestion_in_subject = dir_main.entryList(); //создание листа входящих файлов
            namesOfQuestion_in_subject.removeFirst();
            namesOfQuestion_in_subject.removeFirst(); // Удаляем первые два елемента "."  и ".."

            //Так как в каждой теме вопросы по папкам, для удобства хранения файла и картинки вместе с ним, то заходим в каждую потенциальную папку
            for( int j = 0; j < namesOfQuestion_in_subject.size(); ++j )
            {
                //если не находим точек в названии файла, т.е. это не расширение, то значит это папка с вопросом, значит заходим
                if( namesOfQuestion_in_subject[ j ].indexOf( "." ) == - 1 )
                {
                    dir_main.cd(  namesOfQuestion_in_subject[ j ] ); //Заходим в вопрос
                    QStringList files_in_question = dir_main.entryList(); //Получаем лист файлов в папке с вопросом

                    //ищем в листе файл "Вопрос.txt"
                    for( const auto& files : files_in_question )
                    {
                        //если находим, то увеличиваем общий счетчик и добавляем в мапу
                        if( files == "Вопрос.txt")
                        {
                            ++current_all_question;

                            if( map_topic.find( name_directory ) == map_topic.end() )
                            {
                                map_topic[ name_directory ] = 1;
                            }
                            else
                            {
                                ++map_topic[ name_directory ];
                            }

                            break;
                        }
                    }
                    dir_main.cdUp(); //подымаемся из папки вопроса обратно в папку со всеми вопросами темы
                }
            }
            dir_main.cdUp(); //подымаемся из папки темы в главную папку с темами
        }
     }

     all_count_question_in_directory = current_all_question;
}


//Генерация окна "Инструкция_Тест"
void MainWindow::generateWindow_Instruction_Test()
{

    QTextEdit *te =  qobject_cast<QTextEdit *>( layout->itemAt(1)->widget() );
    FIO = te->toPlainText();
    this->setWindowTitle("Инструкция");

    //Удаляем виджеты с layout
    delete_Widget_Layout();

    //Создать текстовое поле инструкции и найти инструкцию в папке "Вопросы"
    layout->addWidget( get_TextEdit_Instruction() );

    //Создать поле для ввода количества вопросов
    layout->addWidget( get_TextEdit_Count_Question() );

    //Создать кнопку "Начать тест" с проверкой введенных вопросов
    layout->addWidget( get_PushButton_Start_Test() );

    flag_instuction_window = true;
    QTextEdit *tpe =  qobject_cast<QTextEdit *>( layout->itemAt(1)->widget() );
    tpe->installEventFilter( this );
}


//Функция создания и заполнения текстового поля с инструкцией
QTextEdit* MainWindow::get_TextEdit_Instruction()
{
    QTextEdit *result_text_instruction = new QTextEdit; //Создаем текстовое поле и загружаем в него инструкцию из файла

    QFile file( "Вопросы/Инструкция.txt" );
    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream in( &file );
        while ( !in.atEnd() )
        {
            QString line = in.readLine();
            Instruction += line;
            //result_text_instruction->append( line );
        }
        file.close();
    }
    else
    {
        Instruction = "Инструкция не найдена";
    }

    QString setHTML_properties = "font: " + QString::number(  this->size().height() * 5 / 100 < 25 ? 25 :  this->size().height() * 3 / 100  ) + "pt \"Arial\";" +
                                                        "background-color:white;" +
                                                        "" +
                                                        "" +
                                                        "" +
                                                        "" +
                                                        "" +
                                                        "";
    result_text_instruction->setStyleSheet( setHTML_properties );
    result_text_instruction->setFrameStyle( QFrame::NoFrame );
    result_text_instruction->setAlignment( Qt::AlignCenter );
    result_text_instruction->setText( Instruction );
    result_text_instruction->setReadOnly( true );
    return result_text_instruction;
}


//Функция создания текстового поля для заполнения количества вопросов
QTextEdit* MainWindow::get_TextEdit_Count_Question()
{
    QTextEdit *result_text_count_question = new QTextEdit;


    result_text_count_question->setMinimumHeight( 25 );
    result_text_count_question->setMaximumHeight( this->size().height() * 5 / 100 < 25 ? 25 :  this->size().height() * 5 / 100 );

    QString font_size = "font: " + QString::number( ( this->size().height() * 5 / 100 < 25 ? 25 :  this->size().height() * 5 / 100 ) * 1 / 2 - 1 ) + "pt \"Arial\";";
    result_text_count_question->setStyleSheet( font_size );

    result_text_count_question->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    result_text_count_question->setPlaceholderText( "Введите количество вопросов..." );

    return result_text_count_question;
}

//Функция создания кнопки начать текст с привязкой клика
QPushButton* MainWindow::get_PushButton_Start_Test()
{
    QPushButton *result_pushButtom_start_test = new QPushButton;

    result_pushButtom_start_test->setMinimumHeight( this->size().height() * 5 / 100 < 25 ? 25 :  this->size().height() * 5 / 100 );

    QString font_size = "font: " + QString::number( ( this->size().height() * 5 / 100 < 25 ? 25 :  this->size().height() * 5 / 100 ) * 1 / 2 - 1 ) + "pt \"Arial\";"; //+
                                    //"background-color:grey;";
    result_pushButtom_start_test->setStyleSheet( font_size );
    result_pushButtom_start_test->setText( "Начать тест" );

    connect( result_pushButtom_start_test, SIGNAL(clicked()), this, SLOT( on_pushButton_Start_Test_clicked() ) );

    return result_pushButtom_start_test;
}


//Генерация окон самого теста
void MainWindow::generateWindow_Test()
{
    this->setWindowTitle("Тест");
    //Вызвать функцию проверки ответа
    if( !map_random_question.empty() )
        map_random_question[ current_adress ] = check_Answer_Question();

    //Чистим layout
    delete_Widget_Layout();

    if( current_question <= current_count_question )
    {
        ++current_question;
        //Получаем Вопросы/тема/папка с вопросом/
        current_adress = get_Adress_Random_Question();

        //Получаем текстлист из файла
        text_file = get_QStringList_line_in_text( current_adress );

        //Устанавливаем на форму текст вопроса
        set_TextEdit_in_layout();

        //Ставим картинку если есть
        set_PixMap_in_layout();

        //Ставим варианты ответа
        set_Variant_Answer();

        //Ищем верный ответ в файле и сохраняем
        set_Right_Answer();

        //Ставим кнопку "Следующий вопрос"
        set_Button_continue();
    }
    else
    {
        generateWindow_Result();
    }
}

//Получение рандомного вопроса из имеющихся и проверкой на использованный вопрос
QString MainWindow::get_Adress_Random_Question()
{
    QString result;

    bool flag_result = false;
    while( !flag_result )
    {
        qsrand( QDateTime::currentMSecsSinceEpoch() );
        int number_random_question = qrand() % all_count_question_in_directory + 1;

        QString topic_in_directory;
        QMapIterator<QString, int> it( map_topic );
        while( it.hasNext() )
        {
            it.next();
            if( number_random_question > it.value() )
            {
                number_random_question -= it.value();
            }
            else
            {
                topic_in_directory = it.key();
                break;
            }
        }

        //Начинаем поиск вопроса в папке темы
        QDir dir_main;
        dir_main.setPath( "Вопросы/" + topic_in_directory );

        QStringList namesOfQuestion_in_subject = dir_main.entryList(); //создание листа входящих файлов
        if(  namesOfQuestion_in_subject.size() > 2 )
        {
            namesOfQuestion_in_subject.removeFirst();
            namesOfQuestion_in_subject.removeFirst(); // Удаляем первые два елемента "."  и ".."

            //Так как в каждой теме вопросы по папкам, для удобства хранения файла и картинки вместе с ним, то заходим в каждую потенциальную папку

            if( number_random_question > 0 && number_random_question <= namesOfQuestion_in_subject.size() )
            {
                --number_random_question;
                if( namesOfQuestion_in_subject[ number_random_question ].indexOf( "." ) == - 1 )
                {
                    dir_main.cd(  namesOfQuestion_in_subject[ number_random_question ] ); //Заходим в вопрос

                    QStringList files_in_question = dir_main.entryList(); //Получаем лист файлов в папке с вопросом

                    //ищем в листе файл "Вопрос.txt"
                    for( const auto& files : files_in_question )
                    {
                        //если находим, то увеличиваем общий счетчик и добавляем в мапу
                        if( files == "Вопрос.txt")
                        {
                            QString path_current_random_question = "Вопросы/" + topic_in_directory + "/" + namesOfQuestion_in_subject[ number_random_question ] + "/" ;
                            if( map_random_question.find( path_current_random_question ) == map_random_question.end() )
                            {
                                map_random_question[ path_current_random_question ] = false;
                                result = path_current_random_question;
                                flag_result = true;
                            }
                            break;
                        }
                    }
                }
            }
        }

    }

    return  result;
}


//Получение текста из текстового файла Вопрос.txt
QStringList MainWindow::get_QStringList_line_in_text(const QString &adress)
{
    QStringList result;

    QFile file( adress + "Вопрос.txt" );
    if( file.open( QIODevice::ReadOnly ) )
    {
        QTextStream in( &file );
        while ( !in.atEnd() )
        {
            QString line = in.readLine();
            result.push_back( line );
        }
        file.close();
    }

    return  result;
}


//ДОБАВЛЕНИЕ ТЕКСТА ВОПРОСА НА ФОРМУ
void MainWindow::set_TextEdit_in_layout()
{
    QTextEdit *text_edit = new QTextEdit( this );
    QString text_question = "";
    for( const auto& tmp : text_file )
    {
        if( tmp.indexOf( "Адрес картинки:" ) == -1 )
        {
            text_question += tmp + " ";
        }
        else
        {
           break;
        }
    }

    QString setHTML_properties = "font: " + QString::number(  this->size().height() * 5 / 100 < 25 ? 25 :  this->size().height() * 3 / 100  ) + "pt \"Arial\";" +
                                                        "background-color:white;" +
                                                        "" +
                                                        "" +
                                                        "" +
                                                        "" +
                                                        "" +
                                                        "";
    text_edit->setStyleSheet( setHTML_properties );
    text_edit->setFrameStyle( QFrame::NoFrame );
    text_edit->setReadOnly( true );
    text_edit->setText( text_question );
    layout->addWidget( text_edit );
}


void MainWindow::set_PixMap_in_layout() //Вывод картинки на экран( если она есть)
{
    //ДОБАВЛЕНИЕ КАРТИНКИ НА ФОРМУ( ЕСЛИ НАДО )
    QString picture_adress = "";
    for( const auto& tmp : text_file )
    {
        if( tmp.indexOf( "Адрес картинки:" ) != -1 )
        {
            picture_adress = tmp.mid( tmp.indexOf(":") + 1, tmp.size() - 1 - tmp.indexOf(":") ).trimmed();
            break;
        }
    }
    if( picture_adress != "" )
    {
       QString path_picture = "Вопросы/Системы линейных уравнений и их решение методом Гаусса/1/" + picture_adress;
       QPixmap myPixmap( path_picture );
       QLabel *myLabel = new QLabel( this );
       myLabel->setPixmap( myPixmap );
       layout->addWidget( myLabel );
    }
}

//ДОБАВЛЕНИЕ ВАРИАНТОВ ОТВЕТА
void MainWindow::set_Variant_Answer() //Вывод вариантов ответа( открытый закрытый вопрос )
{
     int count_answer = 0;
     for( const auto& tmp : text_file )
     {
         if( tmp.indexOf( "Варианты ответов:" ) != -1 )
         {
             count_answer = tmp.mid( tmp.indexOf(":") + 1, tmp.size() - 1 - tmp.indexOf(":") ).trimmed().toInt();
             break;
         }
     }
    if( count_answer == 0 )
    {
        QTextEdit *text_edit_open_question = new QTextEdit( this );
        text_edit_open_question->setPlaceholderText( "Введите ответ..." );
        layout->addWidget( text_edit_open_question );
    }
    else
    {
        QGroupBox *groupBox = new QGroupBox( this );

        QVector< QString > vector_answer_question;
        bool flag_var_otvet = false;
        for( const auto& tmp : text_file )
        {
            if( tmp.indexOf( "Варианты ответов:" ) != -1 )
            {
                flag_var_otvet = true;
            }
            else if( flag_var_otvet )
            {
                vector_answer_question.push_back( tmp );
            }
        }



        for( int i = 0; i < count_answer; ++i )
        {
            QRadioButton *radio1 = new QRadioButton( );
            radio1->setText( vector_answer_question.at( i ) );
            vec_ref_rad_but.push_back( radio1 );
        }
        QString right_answer = vector_answer_question.at( count_answer );

        QVBoxLayout *vbox = new QVBoxLayout;
        for( const auto& item : vec_ref_rad_but )
        {
            vbox->addWidget( item );
        }
        vbox->addStretch( 3 );
        groupBox->setLayout( vbox );

        layout->addWidget( groupBox );
    }
}


//Уставовка правильного ответа из файла
void MainWindow::set_Right_Answer()
{
    for( const auto& tmp : text_file )
    {
        if( tmp.indexOf( "Ответ:" ) != -1 )
        {
            right_answer = tmp.mid( tmp.indexOf(":") + 1, tmp.size() - 1 - tmp.indexOf(":") ).trimmed();
            break;
        }
    }
}


//ДОБАВЛЕНИЕ ДИНАМИЧЕСКОЙ КОПКИ ДАЛЬШЕ ДЛЯ СЛЕД.ВОПРОСА
void MainWindow::set_Button_continue() //Установка кнопки "Следующий вопрос"
{
     QPushButton *button = new QPushButton(this);  // Создаем объект динамической кнопки
     /* Устанавливаем текст с номером этой кнопки
      * */
     button->setText( "Следующий вопрос" );
     button->setFixedHeight( 25 );
     /* Добавляем кнопку в слой с вертикальной компоновкой
      * */
     layout->addWidget(button);
     /* Подключаем сигнал нажатия кнопки к СЛОТ получения номера кнопки
      * */
     connect(button, SIGNAL(clicked()), this, SLOT( on_pushButton_give_answer() ));
}

//Функция подсчета результатов теста
double MainWindow::set_Result_Test()
{
    double procent_result = 0;
    for( auto it = map_random_question.begin(); it != map_random_question.end(); ++it )
    {
        if( it.value() )
        {
            ++procent_result;
        }
    }

    return  procent_result / map_random_question.size() * 5;
}

//Проверка ответа по текущему вопросу
bool MainWindow::check_Answer_Question()
{
    bool result = false;
    if( vec_ref_rad_but.size() > 0 )
    {
        for( const auto& item : vec_ref_rad_but )
        {
            if( item->isChecked() )
            {
                if( item->text() == right_answer )
                {
                    result = true;
                }
            }
        }
        vec_ref_rad_but.clear();
    }
    else
    {

        QTextEdit *open_answer = qobject_cast< QTextEdit *>(  layout->itemAt( 1 )->widget() );
        if( open_answer )
        {
            answer_quest = open_answer->toPlainText();
        }
        else
        {
            open_answer = qobject_cast< QTextEdit *>(  layout->itemAt( 2 )->widget() );
            if( open_answer )
            {
                answer_quest = open_answer->toPlainText();
            }
        }

        if( answer_quest == right_answer )
        {
            result = true;
        }
    }
    return  result;
}



//Генерация окна с результатом
void MainWindow::generateWindow_Result()
{
    this->setWindowTitle( "Результат" );
    delete_Widget_Layout();

    QTextEdit *text_result = new QTextEdit;

    text_result->append( FIO + "\n" );
    text_result->append( "Ваш результат = " + QString::number( set_Result_Test() ) + "\n" );

    for( auto it = map_random_question.begin(); it != map_random_question.end(); ++it )
    {
        text_result->append( it.key() + " = " + ( it.value() == true ? "Верно" : "Неверно" ) + "\n" );
    }

    layout->addWidget( text_result );

    map_topic.clear();
    map_random_question.clear();


}

//ОБЩИЙ ФУНКЦИОНАЛ
//Функция удаления виджетов с layout
void MainWindow::delete_Widget_Layout()
{
    QLayout* scrollLayout = layout;
    while (QLayoutItem* item = scrollLayout->takeAt(0))
    {
        delete item->widget();
        delete item;
    }
}

//Функция вывода информирующего сообщения на экран
void MainWindow::message_Box_Warning( const QString &message )
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Внимание");
    msgBox.setText( message );
    msgBox.exec();
}

//Отлавливание ентера на первых двух окнах
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if( flag_input_window )
    {

        if( event->type() == QKeyEvent::KeyPress )
        {

            QTextEdit *te =  qobject_cast<QTextEdit *>( layout->itemAt(1)->widget() );
            if( te )
            {
                if( watched == te )
                {
                    QKeyEvent * ke = static_cast<QKeyEvent*>(event);
                    if(ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter)
                    {


                        return true; // do not process this event further
                    }
                }
                return false; // process this event further
            }
            else
            {
                // pass the event on to the parent class
                return QMainWindow::eventFilter(watched, event);
            }
        }
    }
    else if( flag_instuction_window )
    {

        if(event->type() == QKeyEvent::KeyPress)
        {
            QTextEdit *te =  qobject_cast<QTextEdit *>( layout->itemAt(1)->widget() );
            if( te )
            {
                if( watched == te )
                {
                    QKeyEvent * ke = static_cast<QKeyEvent*>(event);
                    if(ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter)
                    {


                        return true; // do not process this event further
                    }
                }
                return false; // process this event further
            }
            else
            {
                // pass the event on to the parent class
                return QMainWindow::eventFilter(watched, event);
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}


void MainWindow::resizeEvent(QResizeEvent *e)
{
    if( flag_input_window )
    {
        resizeInpWin_welcom_te( e );
        resizeInpWin_fio_te( e );
        resizeInpWin_continue_pb( e );

    }
    else if( flag_instuction_window )
    {
         resizeInstructWin_instuction_te( e );
        resizeInstructWin_count_te( e );
        resizeInstructWin_start_pb( e );
    }
}

//Функции перерисовки окна входа
//Перерисовка поля "Добро пожаловать"(первое)
void MainWindow::resizeInpWin_welcom_te( QResizeEvent *e )
{
    QTextEdit *wte =  qobject_cast<QTextEdit *>( layout->itemAt( 0 )->widget() );

    QString setHTML_properties = "font: " + QString::number(  e->size().height() * 5 / 100 < 25 ? 25 :  e->size().height() * 5 / 100  ) + "pt \"Arial\";" +
                                                        "background-color:white;" +
                                                        "" +
                                                        "" +
                                                        "" +
                                                        "" +
                                                        "" +
                                                        "";
    wte->setStyleSheet( setHTML_properties );
    wte->setFrameStyle( QFrame::NoFrame );
    wte->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    wte->setPlainText( "Добро пожаловать!");
    wte->setAlignment( Qt::AlignCenter );
}
//Перерисовка поля ввода ФИО(второе)
void MainWindow::resizeInpWin_fio_te( QResizeEvent *e )
{
    QTextEdit *te =  qobject_cast<QTextEdit *>( layout->itemAt(1)->widget() );

    te->setMinimumHeight( 25 );
    te->setMaximumHeight( e->size().height() * 5 / 100 < 25 ? 25 :  e->size().height() * 5 / 100 );

    QString font_size = "font: " + QString::number( ( e->size().height() * 5 / 100 < 25 ? 25 :  e->size().height() * 5 / 100 ) * 1 / 2 - 1 ) + "pt \"Arial\";";
    te->setStyleSheet( font_size );

    te->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    te->setPlaceholderText( "Введите ФИО..." );
}
//Перерисовка кнопки "Продолжить"
void MainWindow::resizeInpWin_continue_pb( QResizeEvent *e )
{
    QPushButton *pb =  qobject_cast<QPushButton *>( layout->itemAt( 2 )->widget() );
    pb->setMinimumHeight( e->size().height() * 5 / 100 < 25 ? 25 :  e->size().height() * 5 / 100 );

    QString font_size = "font: " + QString::number( ( e->size().height() * 5 / 100 < 25 ? 25 :  e->size().height() * 5 / 100 ) * 1 / 2 - 1 ) + "pt \"Arial\";";
    pb->setStyleSheet( font_size );
}

//Функции перерисовки окна инструкции
//Функция перерисовки поля с инструкцией
void MainWindow::resizeInstructWin_instuction_te( QResizeEvent *e )
{
    QTextEdit *ite =  qobject_cast<QTextEdit *>( layout->itemAt( 0 )->widget() );

    QString setHTML_properties = "font: " + QString::number(  e->size().height() * 5 / 100 < 25 ? 25 :  e->size().height() * 3 / 100  ) + "pt \"Arial\";" +
                                                        "background-color:white;" +
                                                        "" +
                                                        "" +
                                                        "" +
                                                        "" +
                                                        "" +
                                                        "";
    ite->setStyleSheet( setHTML_properties );
    ite->setFrameStyle( QFrame::NoFrame );
    //ite->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ite->setPlainText( Instruction );
    ite->setAlignment( Qt::AlignCenter );
}
//Функция перерисовка поля ввода количества вопросов
void MainWindow::resizeInstructWin_count_te( QResizeEvent *e )
{
    QTextEdit *te =  qobject_cast<QTextEdit *>( layout->itemAt(1)->widget() );

    te->setMinimumHeight( 25 );
    te->setMaximumHeight( e->size().height() * 5 / 100 < 25 ? 25 :  e->size().height() * 5 / 100 );

    QString font_size = "font: " + QString::number( ( e->size().height() * 5 / 100 < 25 ? 25 :  e->size().height() * 5 / 100 ) * 1 / 2 - 1 ) + "pt \"Arial\";";
    te->setStyleSheet( font_size );

    te->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    te->setPlaceholderText( "Введите количество вопросов..." );
}
//Функция перерисовка кнопки "Начать тест"
void MainWindow::resizeInstructWin_start_pb( QResizeEvent *e )
{
    QPushButton *pb =  qobject_cast<QPushButton *>( layout->itemAt( 2 )->widget() );
    pb->setMinimumHeight( e->size().height() * 5 / 100 < 25 ? 25 :  e->size().height() * 5 / 100 );

    QString font_size = "font: " + QString::number( ( e->size().height() * 5 / 100 < 25 ? 25 :  e->size().height() * 5 / 100 ) * 1 / 2 - 1 ) + "pt \"Arial\";";
    pb->setStyleSheet( font_size );
}

//КНОПКИ
//Функция нажатия кнопки продолжить
void MainWindow::on_pushButton_continue_clicked()
{
    QTextEdit *te =  qobject_cast<QTextEdit *>( layout->itemAt(1)->widget() );
    if( te )
    {
        if( te->toPlainText().size() == 0 )
        {
            message_Box_Warning( "Введите ФИО      ");
        }
        else
        {
            flag_input_window = false;
            flag_instuction_window = true;
            generateWindow_Instruction_Test();
        }
    }
}

//Функция нажатия кнопки "Начать тест"
void MainWindow::on_pushButton_Start_Test_clicked()
{
    //Приведение элемента layout к TextEdit и приведение к числу текста в элементе
    QTextEdit *te =  qobject_cast<QTextEdit *>( layout->itemAt(1)->widget() );
    if (te)
        current_count_question = te->toPlainText().toInt();

    //Если введено не положительное количество вопросов или некореектное значение
    if( current_count_question <= 0 )
    {
        message_Box_Warning( "Введите количество вопросов" );
    }
    else if( current_count_question > all_count_question_in_directory )
    {
        message_Box_Warning( "В системе всего " + QString::number( all_count_question_in_directory ) + " вопросов." );
    }
    else
    {
        //Запускаем тест
        flag_instuction_window = false;
        generateWindow_Test();

    }
}


//Функция нажатия кнопки "Следующий вопрос"
void MainWindow::on_pushButton_give_answer()
{
    generateWindow_Test();
}
