#include "textform.h"
#include "ui_textform.h"

TextForm::TextForm(QWidget *parent) : QWidget(parent),
                                      ui(new Ui::TextForm)
{
    ui->setupUi(this);
    connect(this, SIGNAL(value(QString)), ui->textEdit, SLOT(appendText(QString)));
}

TextForm::~TextForm()
{
    delete ui;
}

void TextForm::appendText(QString text)
{
    ui->textEdit->append(text);
}
void TextForm::textChanged(QString Text)
{
    Text = mytcpsocket->showMsg();
    emit value(Text);
}