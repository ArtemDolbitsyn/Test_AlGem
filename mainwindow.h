#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include <QFont>
#include <QSet>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_continue_clicked(); //Обработчик кнопки "Продолжить" первого окна
    void on_pushButton_Start_Test_clicked(); //Обработчик кнопки "Начать тест" первого окна
    void on_pushButton_give_answer(); //Обработчик кнопки дать ответ при каждом вопросе теста

protected:

private:
    Ui::MainWindow *ui;

    //ФУНКЦИИ КЛАССА
    //ОКНА
    //Генерация окна входа в тест
    void generateWindow_Input(); //Функция генрации окна входа в тест
    void Fill_Map_Topic(); //Заполнение мапы тем


    //Генерация окна начала теста
    void generateWindow_Instruction_Test(); //Функция генерации окна начала теста
    QTextEdit* get_TextEdit_Instruction(); //Поиск инструкции, создание текстового виджета и загрузка его в layout
    QTextEdit* get_TextEdit_Count_Question(); //Создание поля количества вопросов
    QPushButton* get_PushButton_Start_Test();   //Создание кнопки начать тест с привязкой клика


    //Генерация окон теста
    void generateWindow_Test(); //Функция генерации окон теста
    QString get_Adress_Random_Question();
    QStringList get_QStringList_line_in_text( const QString& adress );
    void set_TextEdit_in_layout(); //Вывод текста вопроса на экран
    void set_PixMap_in_layout(); //Вывод картинки на экран( если она есть)
    void set_Variant_Answer(); //Вывод вариантов ответа( открытый закрытый вопрос )
    void set_Right_Answer(); //Установка правильного ответа в строку
    void set_Button_continue(); //Установка кнопки "Следующий вопрос
    double set_Result_Test(); //Функция подсчета результатов теста
    bool check_Answer_Question(); //Функция проверки правильного ответа

    //Генерация вывода результата теста
    void generateWindow_Result();

    //ОБЩИЙ ФУНКЦИОНАЛ
    void delete_Widget_Layout(); //Функция удаления виджетов с layout
    void message_Box_Warning( const QString& message ); //Функция вывода сообщения "Внимание"
    bool eventFilter(QObject *watched, QEvent *event); //Отлавливание ввода "Enter" на первых двух окнах


    void resizeEvent(QResizeEvent *e); //Функция перерисовывание при изменении размеров окна
    //Функции перерисовки окна входа
    void resizeInpWin_welcom_te( QResizeEvent *e ); //Перерисовка поля "Добро пожаловать"(первое)
    void resizeInpWin_fio_te( QResizeEvent *e );//Перерисовка поля ввода ФИО(второе)
    void resizeInpWin_continue_pb( QResizeEvent *e );//Перерисовка кнопки "Продолжить"
    //Функции перерисовки окна инструкции
    void resizeInstructWin_instuction_te( QResizeEvent *e );
    void resizeInstructWin_count_te( QResizeEvent *e );
    void resizeInstructWin_start_pb( QResizeEvent *e );
    //Функции перерисовки окон теста
    void resizeTestWin_text_question( QResizeEvent *e );
    void resizeTestWin_contpb( QResizeEvent *e );


    //ПОЛЯ КЛАССА
    QWidget *window = new QWidget; //Само окно
    QVBoxLayout *layout = new QVBoxLayout;



    bool flag_input_window = false;
    bool flag_instuction_window = false;
    bool flag_test_window = false;

    QString FIO;
    QString Instruction;


    QMap< QString, QSet< QString > > map_topic; //мапа тем
    int all_count_question_in_directory = 0; //общее количество вопросов

    int current_count_question; //количество вопросов в создаваемом тесте
    int current_question = 0; //текущий вопрос

    QString right_answer;

    QString topic_in_directory;
    QString current_adress;
    //QMap< QString, QString > map_random_question; //мапа рандомных вопросов генерируемых в тесте c ответам
    QList< QPair< QString, bool > > list_random_question;
    QMap< QString, QPair< QString, QString > > map_answer;
    QImage *image_class;

    QStringList text_file; //лист строк в файле

    //QVector< QRadioButton* > vec_ref_rad_but;
    QVector< QPair < QRadioButton*, QString > > vec_ref_rad_but_and_text;
    QString answer_quest;

    //Поле размера пикселей для читаемого текста
    int current_size_pixel_read_text;
};
#endif // MAINWINDOW_H
