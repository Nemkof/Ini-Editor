#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":images//img//in.png"));
    setInitialization();
    setBlock();

    // Валидаторы
    intValid = new QIntValidator();
    doubleValid = new QDoubleValidator();

    QRegularExpression expr("1|0|true|false");
    boolValid = new QRegularExpressionValidator(expr);
}

// Начальная иницилизация
void MainWindow::setInitialization(){
    // treeWidget
    ui->treeWidget->setColumnCount(2);
    QStringList labels;
    labels << "Sections/Keys" << "Values";
    ui->treeWidget->setHeaderLabels(labels);

    // horizontalLayout_1
    ui->label_sections->setText("Sections");

    // horizontalLayout_2
    // groupeBox_adding
    ui->groupBox_add->setTitle("Adding");
    ui->label_key_add->setText("Key");
    ui->label_type->setText("Type");
    ui->label_value->setText("Value");
    ui->button_add->setText("Add");

    // groupBox_deleting
    ui->groupBox_del->setTitle("Deleting");
    ui->label_key_del->setText("Key");
    ui->button_delete->setText("Delete");

    // Иконка на кнопку "плюс"
    QPixmap add_1(":/images/img/plus.png");
    QIcon ButtonPlus(add_1);
    ui->button_Plus->setIcon(ButtonPlus);
    // Иконка на кнопку "минус"
    QPixmap add_2(":/images/img/minus.png");
    QIcon ButtonMinus(add_2);
    ui->button_Minus->setIcon(ButtonMinus);
}

// Заблокируем кнопочки
void MainWindow:: setBlock(){
    ui->button_add->setEnabled(false);
    ui->button_delete->setEnabled(false);

    ui->label_key_add->setEnabled(false);
    ui->button_delete->setEnabled(false);

    ui->comboBox_sections->setEnabled(false);
    ui->comboBox_key->setEnabled(false);
    ui->comboBox_type->setEnabled(false);

    ui->lineEdit_key->setEnabled(false);
    ui->lineEdit_value->setEnabled(false);

    ui->button_Plus->setEnabled(false);
    ui->button_Minus->setEnabled(false);
}

// Разблокируем кнопочки
void MainWindow:: setUnblock(){
    ui->button_add->setEnabled(true);
    ui->button_delete->setEnabled(true);

    ui->label_key_add->setEnabled(true);
    ui->button_delete->setEnabled(true);

    ui->comboBox_sections->setEnabled(true);
    ui->comboBox_key->setEnabled(true);
    ui->comboBox_type->setEnabled(true);

    ui->lineEdit_key->setEnabled(true);
    ui->lineEdit_value->setEnabled(true);

    ui->button_Plus->setEnabled(true);
    ui->button_Minus->setEnabled(true);
}

// Деструктор
MainWindow::~MainWindow()
{
    delete ui;
    delete intValid;
    delete doubleValid;
    delete boolValid;
    delete myIniFile;
}

// Очистим дерево
void MainWindow::setClear()
{
    ui->treeWidget->clear();
    ui->comboBox_sections->clear();
    ui->comboBox_type->clear();
    ui->comboBox_key->clear();

    ui->lineEdit_key->clear();
    ui->lineEdit_value->clear();
}

// Открываем файл
void MainWindow::on_actionOpen_file_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open File",
             QDir::currentPath(), "Ini files (*.ini)");

    //Собираем данные из ListWidget
    std::set <QString> texts;
    int n = ui->listWidget->count();
    for (int i = 0; i < n ; ++i){
        QModelIndex *model_index = new QModelIndex(ui->listWidget->model()->index(i,0));
        QString q = model_index->data(Qt::DisplayRole).toString();
        texts.insert(q);
    }
    // проверка, что этот файл уже был выбран
    if (texts.count(filePath) == 0){
        ui->listWidget->addItem(filePath);
    }
    else{
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Этот файл уже выбран!");
        msgBox.exec();
    }
    texts.clear();
}

// Открываем директорию
void MainWindow::on_actionOpen_directory_triggered()
{
    QString directoryPath = QFileDialog::getExistingDirectory(this,
                    tr("Choose Directory"),QDir::currentPath(),
                    QFileDialog::DontResolveSymlinks);
    QDir dir(directoryPath);

    if (dir.isEmpty()){
        // Проверка, что директория может не содержать файлы
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("В этой директории нет файлов!");
        msgBox.exec();
    }else{
        QFileInfoList list = dir.entryInfoList();
        ui->listWidget->clear();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            if (!fileInfo.fileName().isEmpty() && (fileInfo.fileName().indexOf(".ini") != -1)) {
                ui->listWidget->addItem(fileInfo.path() + "/" + fileInfo.fileName());
            }
        }
    }
}

// Заполняем окно информацией файла
void MainWindow::setInfoOfFile()
{
    // Очищаем дерево
    setClear();

    // Заполняем comboBox_type
    std::set<std::string> AddingType {"int", "double", "bool", "string"};
    for (const auto& n : AddingType){
        ui->comboBox_type->addItem(QString::fromStdString(n));
    }
    // Заполняем treeWidget и comboBox_sections
    for (const auto &it : myIniFile->getSections()) {

        // Создаём item-секцию - потомка от treeWidget
        QTreeWidgetItem *root = new QTreeWidgetItem(ui->treeWidget);

        // Помещаем в неё название секции
        root->setText(0, QString::fromStdString(it.first));

        // Помещаем секцию в tree
        ui->treeWidget->addTopLevelItem(root);

        // Помещаем название секции в comboBox_sections
        ui->comboBox_sections->addItem(QString::fromStdString(it.first));

        // Идём по всем ключам секции
        for (const auto &it_2 : it.second) {
            // Создаём item-ключ-значение - потомок от секции
            QTreeWidgetItem *child = new QTreeWidgetItem();
            // Помещаем в него ключ и значение
            child->setText(0, QString::fromStdString(it_2.first));//ключ
            child->setText(1, QString::fromStdString(it_2.second)); //значение
            // Добавляем в секцию нашу пару ключ-значение
            root->addChild(child);
        }
    }
}

// Кликаем на файл в списке
void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    // Берём адрес файла, открываем его
    QString fileName = item->text();
    myIniFile = new IniFile(fileName.toStdString());

    // Разблокируем кнопки, заполним информацию о файле
    setUnblock();
    setInfoOfFile();
}

// Добавляем секцию
void MainWindow::on_button_Plus_clicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Adding section"),
    tr("Enter the name of the section:"), QLineEdit::Normal, QDir::home().dirName(), &ok);
    // Попытка создать секцию без имени
    if (ok && name.isEmpty()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Название секции не может быть пусто!");
        msgBox.exec();
    }
    // Такая секция уже есть
    else if (ok && myIniFile->isSectionExist(name.toStdString())){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Такая секция уже существует!");
        msgBox.exec();
    }
    // Создаём секцию
    else if (ok){
        // Добавляем секцию в файл
        myIniFile->addNewSection(name.toStdString());

        // Добавляем секцию в comboBox_sections и обновляем его
        ui->comboBox_sections->addItem(name);

        // Обновляем дерево файла
        setInfoOfFile();
    }
}

// Удаляем секцию, которая была выбрана в comboBox_sections
void MainWindow::on_button_Minus_clicked()
{
    // Получаем имя секции из comboBox
    QString name = ui->comboBox_sections->currentText();
    // Удаляем секцию из файла
    myIniFile->deleteSection(name.toStdString());
    // Обновляем дерево
    setInfoOfFile();
}

// Кликаем на какую-то секцию/ключ/значение
void MainWindow::on_treeWidget_itemSelectionChanged()
{
    // Меняем секцию в box
    QTreeWidgetItem* temp = ui->treeWidget->currentItem();

    // Если колонка 1 пустая, то мы кликнули на секцию
    // Поменяем секцию в comboBox_sections
    if (temp->text(1).isEmpty()) {
        ui->comboBox_sections->setCurrentText(temp->text(0));
    }

    // Если колонка 1 не пустая, то мы кликнули на пару ключ-значение
    // Поменяем секцию и ключ в двух box
    else {
        ui->comboBox_sections->setCurrentText(temp->parent()->text(0));
        ui->comboBox_key->setCurrentText(temp->text(0));
    }
}

// Замечено изменение в comboBox_sections
void MainWindow::on_comboBox_sections_currentTextChanged(const QString &arg1)
{
    // Очищаем box с ключами
    ui->comboBox_key->clear();

    // Берём мапу со всеми секциями и ключами
    SectionsMap sectionInfo = myIniFile->getSections();

    // Проходим по всем ключам выбранной секции и добавляем их в box с ключами
    for (const auto &element : sectionInfo[arg1.toStdString()]){
        ui->comboBox_key->addItem(QString::fromStdString(element.first));
    }
}

// Замечено изменение в comboBox_type - меняем валидаторы
void MainWindow::on_comboBox_type_currentTextChanged(const QString &arg1)
{
    ui->lineEdit_key->clear();
    ui->lineEdit_value->clear();

    if (arg1 == "int") {
        ui->lineEdit_value->setValidator(intValid);
    }
    else if (arg1 == "double") {
        ui->lineEdit_value->setValidator(doubleValid);
    }
    else if (arg1 == "bool") {
        ui->lineEdit_value->setValidator(boolValid);
    }
    else {
        ui->lineEdit_value->setValidator(0);
    }
}

// Добавляем пару ключ-значение
void MainWindow::on_button_add_clicked()
{
    // Считываем всё, что нужно
    QString section = ui->comboBox_sections->currentText();
    QString type_value = ui->comboBox_type->currentText();
    QString key = ui->lineEdit_key->text();
    QString value = ui->lineEdit_value->text();

    // Ключ пустой
    if (key.isEmpty()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Вы не ввели название ключа!");
        msgBox.exec();
    }
    // Значение пустое
    else if (value.isEmpty()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Вы не ввели значение!");
        msgBox.exec();
    }
    else {
        if (type_value == "int"){
            myIniFile->writeInt(section.toStdString(), key.toStdString(),  value.toInt());
        }else if (type_value == "double"){
            std::string str = value.toStdString();

            // Меняем все запятые на точки
            std::replace(str.begin(), str.end(), ',', '.');

            myIniFile->writeDouble(section.toStdString(), key.toStdString(), stod(str));
        }
        else if(type_value == "string"){
            myIniFile->writeString(section.toStdString(), key.toStdString(), value.toStdString());
        }
        else{
            if ((type_value == "true") || (type_value == "1")){
                myIniFile->writeString(section.toStdString(), key.toStdString(), value.toStdString());
            }else{
                myIniFile->writeString(section.toStdString(), key.toStdString(), value.toStdString());
            }
        }
        // Обновляем дерево
        setInfoOfFile();
    }
}

// Удаляем ключ
void MainWindow::on_button_delete_clicked()
{
    QString section = ui->comboBox_sections->currentText();
    QString key = ui->comboBox_key->currentText();

    if (key.isEmpty()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Удалять нечего");
        msgBox.exec();
    }
    else{
        myIniFile->deleteKey(section.toStdString(), key.toStdString());
        setInfoOfFile();
    }
}

// Сохраняем файл
void MainWindow::on_actionSave_triggered()
{
    // Нечего сохранять
    if (ui->listWidget->count() == 0){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Нет открытых файлов!");
        msgBox.exec();
    }
    else
    {
        myIniFile->save();
    }
}
