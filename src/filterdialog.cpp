/**
 * @file filterdialog.cpp
 * @author Ján Bella <xbella1@fi.muni.cz>
 */

#include "filterdialog.h"

using namespace FT1D;

FilterDialog::FilterDialog(FilterType type, Signal& magnitude, const Translation* language, QWidget *parent)
    : QDialog(parent), translation(language), magnitude(magnitude)
{
    maxFrequency = magnitude.original_length() / 2;

    setEnabled(true);

    setFixedSize(QSize(650, 250));
    setModal(true);

    filterName = new QLabel(this);
    filterName->setGeometry(0,5,650,25);
    filterName->setAlignment(Qt::AlignCenter);

    QFont font;
    font.setPointSize(14);
    filterName->setFont(font);

    filterFormula = new QLabel(this);
    filterFormula->setGeometry(10,35,300,25);
    filterFormula->setAlignment(Qt::AlignCenter);

    plot = new QCustomPlot(this);
    plot->setGeometry(QRect(10, 70, 290, 170));
    plot->setCursor(QCursor(Qt::CrossCursor));


    plot->xAxis->setRange(0, maxFrequency + 1);
    plot->yAxis->setRange(-0.1, 1.2);

    plot->yAxis->setNumberFormat("f");
    plot->yAxis->setNumberPrecision(2);

    plot->xAxis->setNumberFormat("f");
    plot->xAxis->setNumberPrecision(0);

    plot->xAxis->setAutoTickStep(false);
    plot->xAxis->setTickStep(2);

    QLabel* plotxAxisLabel = new QLabel("ω",plot);
    plotxAxisLabel->setGeometry(275,143,20,20);

    QLabel* plotyAxisLabel = new QLabel("G(ω)",plot);
    plotyAxisLabel->setGeometry(4,0,34,20);

    plot->addGraph();
    plot->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));

    firstPreferenceLabel = new QLabel(this);
    firstPreferenceLabel->setVisible(false);
    firstPreferenceLabel->setGeometry(320, 70, 25, 30);

    secondPreferenceLabel = new QLabel(this);
    secondPreferenceLabel->setVisible(false);
    secondPreferenceLabel->setGeometry(320, 110, 25, 30);

    firstLineEdit = new QLineEdit(this);
    firstLineEdit->setVisible(false);
    firstLineEdit->setGeometry(345, 70, 45, 30);

    secondLineEdit = new QLineEdit(this);
    secondLineEdit->setVisible(false);
    secondLineEdit->setGeometry(345, 110, 45, 30);

    firstFrontLabel = new QLabel(this);
    firstFrontLabel->setVisible(false);
    firstFrontLabel->setGeometry(400, 70, 10, 30);
    firstFrontLabel->setText(QString::number(0));

    secondFrontLabel = new QLabel(this);
    secondFrontLabel->setVisible(false);
    secondFrontLabel->setGeometry(400, 110, 10, 30);
    secondFrontLabel->setText(QString::number(0));

    firstSlider = new QSlider(Qt::Orientation::Horizontal,this);
    firstSlider->setGeometry(QRect(410, 70, 200, 30));
    firstSlider->setRange(0,maxFrequency);
    firstSlider->setVisible(false);

    secondSlider = new QSlider(Qt::Orientation::Horizontal,this);
    secondSlider->setGeometry(QRect(410, 110, 200, 30));
    secondSlider->setRange(0,maxFrequency);
    secondSlider->setVisible(false);

    firstBackLabel = new QLabel(this);
    firstBackLabel->setVisible(false);
    firstBackLabel->setGeometry(QRect(615, 70, 20, 30));

    secondBackLabel = new QLabel(this);
    secondBackLabel->setVisible(false);
    secondBackLabel->setGeometry(QRect(615, 110, 20, 30));

    spinBox = new QSpinBox(this);
    spinBox->setVisible(false);
    spinBox->setGeometry(345,110,45,30);

    okButton = new QPushButton(QStringLiteral("OK"), this);
    okButton->setGeometry(540,210,100,30);
    cancelButton = new QPushButton(QStringLiteral("Cancel"),this);
    cancelButton->setGeometry(430,210,100,30);

    connect(cancelButton,&QPushButton::clicked,this,[=](bool)
    {
        reject();
    });


    switch(type)
    {
    case ILPF:
        initLowPass();
        break;
    case IHPF:
        initHighPass();
        break;
    case BANDPASS:
        initBandPass();
        break;
    case LPGAUSS:
        initGaussianLowPass();
        break;
    case HPGAUSS:
        initGaussianHighPass();
        break;
    case LPBUTTERWORTH:
        initButterworthLowPass();
        break;
    case HPBUTTERWORTH:
        initButterworthHighPass();
        break;
    default:
        reject();
        break;
    }

    setLocalizedTexts(language, type);
}


void FilterDialog::initLowPass()
{
    connect(firstSlider,&QSlider::valueChanged, this, [=](int value)
    {
        firstLineEdit->setText(QString::number(value));
        ilpfGraph(value);
    });

    connect(okButton,&QPushButton::clicked,this,[=](bool)
    {
        idealLowPassFilter(firstSlider->value());
        accept();
    });

    connect(firstLineEdit, &QLineEdit::textEdited,this,[=](QString text)
    {
        bool ok;
        int num = text.toInt(&ok);
        if(!ok || num < 0 || num > maxFrequency)
        {
            firstLineEdit->setText(QString::number(firstSlider->value()));
        }
        else
        {
            firstSlider->setValue(num);
        }
    });


    filterName->setText("Ideal low-pass filter");
    filterFormula->setText("G(ω) = { 1 if ω ≤ ω₀; else 0 }");

    firstPreferenceLabel->setVisible(true);
    firstPreferenceLabel->setText(QStringLiteral("ω₀: "));

    firstLineEdit->setVisible(true);
    firstLineEdit->setText(QString::number(1));

    firstFrontLabel->setVisible(true);
    firstFrontLabel->setText(QStringLiteral("0"));

    firstSlider->setVisible(true);
    firstSlider->setValue(1);

    firstBackLabel->setVisible(true);
    firstBackLabel->setText(QString::number(maxFrequency));



}

void FilterDialog::initHighPass()
{
    connect(firstSlider,&QSlider::valueChanged, this, [=](int value)
    {
        firstLineEdit->setText(QString::number(maxFrequency - value));
        ihpfGraph(maxFrequency - value);
    });

    connect(okButton,&QPushButton::clicked,this,[=](bool)
    {
        idealHighPassFilter(maxFrequency - firstSlider->value());
        accept();
    });

    connect(firstLineEdit, &QLineEdit::textEdited,this,[=](QString text)
    {
        bool ok;
        int num = text.toInt(&ok);
        if(!ok || num < 0 || num > maxFrequency)
        {
            firstLineEdit->setText(QString::number(maxFrequency - firstSlider->value()));
        }
        else
        {
            firstSlider->setValue(maxFrequency - num);
        }
    });

    filterName->setText("Ideal high-pass filter");
    filterFormula->setText("G(ω) = { 1 if ω ≥ ω₀; else 0 }");

    firstPreferenceLabel->setVisible(true);
    firstPreferenceLabel->setText(QStringLiteral("ω₀: "));

    firstLineEdit->setVisible(true);
    firstLineEdit->setText(QString::number(1));

    firstFrontLabel->setVisible(true);
    firstFrontLabel->setText(QStringLiteral("0"));

    firstSlider->setVisible(true);
    firstSlider->setRange(0,maxFrequency);
    firstSlider->setValue(1);
    firstSlider->setInvertedAppearance(true);

    firstBackLabel->setVisible(true);
    firstBackLabel->setText(QString::number(maxFrequency));


}

void FilterDialog::initBandPass()
{
    connect(firstSlider,&QSlider::valueChanged, this, [=](int value)
    {
        if(secondSlider->value() < maxFrequency - value)
        {
            secondSlider->setSliderPosition(maxFrequency - value);
        }

        firstLineEdit->setText(QString::number(maxFrequency - value));
        bpfGraph(maxFrequency - firstSlider->value(), secondSlider->value());
    });

    connect(secondSlider,&QSlider::valueChanged, this, [=](int value)
    {
        if(maxFrequency - firstSlider->value() > value)
        {
            firstSlider->setSliderPosition(maxFrequency - value);
        }

        secondLineEdit->setText(QString::number(value));
        bpfGraph(maxFrequency - firstSlider->value(), secondSlider->value());
    });


    connect(okButton,&QPushButton::clicked,this,[=](bool)
    {
        bandPassFilter(firstLineEdit->text().toInt(), secondLineEdit->text().toInt());
        accept();
    });

    connect(firstLineEdit, &QLineEdit::textEdited,this,[=](QString text)
    {
        bool ok;
        int num = text.toInt(&ok);
        if(!ok || num < 0 || num > maxFrequency)
        {
            firstLineEdit->setText(QString::number(firstSlider->value()));
        }
        else
        {
            firstSlider->setValue(maxFrequency - num);
        }
    });

    connect(secondLineEdit, &QLineEdit::textEdited,this,[=](QString text)
    {
        bool ok;
        int num = text.toInt(&ok);
        if(!ok || num < 0 || num > maxFrequency)
        {
            secondLineEdit->setText(QString::number(secondSlider->value()));
        }
        else
        {
            secondSlider->setValue(num);
        }
    });


    filterName->setText("Ideal band-pass filter");
    filterFormula->setText("G(ω) = { 1 if ω₁ ≤ ω ≤ ω₂; else 0 }");

    firstPreferenceLabel->setVisible(true);
    firstPreferenceLabel->setText(QStringLiteral("ω₁: "));

    firstLineEdit->setVisible(true);
    firstLineEdit->setText(QString::number(1));

    firstFrontLabel->setVisible(true);
    firstFrontLabel->setText(QStringLiteral("0"));

    firstSlider->setVisible(true);
    firstSlider->setRange(0,maxFrequency);
    firstSlider->setValue(maxFrequency - 1);
    firstSlider->setInvertedAppearance(true);

    firstBackLabel->setVisible(true);
    firstBackLabel->setText(QString::number(maxFrequency));


    secondPreferenceLabel->setVisible(true);
    secondPreferenceLabel->setText(QStringLiteral("ω₂: "));

    secondLineEdit->setVisible(true);
    secondLineEdit->setText(QString::number(maxFrequency - 1));

    secondFrontLabel->setVisible(true);
    secondFrontLabel->setText(QStringLiteral("0"));

    secondSlider->setVisible(true);
    secondSlider->setRange(0, maxFrequency);
    secondSlider->setValue(maxFrequency - 1);


    secondBackLabel->setVisible(true);
    secondBackLabel->setText(QString::number(maxFrequency));
}

void FilterDialog::initGaussianLowPass()
{
    connect(firstSlider,&QSlider::valueChanged, this, [=](int value)
    {
        firstLineEdit->setText(QString::number(value));
        glpfGraph(value);
    });

    connect(okButton,&QPushButton::clicked,this,[=](bool)
    {
        gaussianLowPassFilter(firstSlider->value());
        accept();
    });

    connect(firstLineEdit, &QLineEdit::textEdited,this,[=](QString text)
    {
        bool ok;
        int num = text.toInt(&ok);
        if(!ok || num < 0 || num > maxFrequency)
        {
            firstLineEdit->setText(QString::number(firstSlider->value()));
        }
        else
        {
            firstSlider->setValue(num);
        }
    });

    filterName->setText("Guassian low-pass filter");
    filterFormula->setText("G(ω) = exp(-ω²/2ω₀²)"); // c·

    firstPreferenceLabel->setVisible(true);
    firstPreferenceLabel->setText(QStringLiteral("ω₀: "));

    firstLineEdit->setVisible(true);
    firstLineEdit->setText(QString::number(1));

    firstFrontLabel->setVisible(true);
    firstFrontLabel->setText(QStringLiteral("0"));

    firstSlider->setVisible(true);
    firstSlider->setRange(0,maxFrequency);
    firstSlider->setValue(1);

    firstBackLabel->setVisible(true);
    firstBackLabel->setText(QString::number(maxFrequency));
}

void FilterDialog::initGaussianHighPass()
{
    connect(firstSlider,&QSlider::valueChanged, this, [=](int value)
    {
        firstLineEdit->setText(QString::number(maxFrequency - value));
        ghpfGraph(maxFrequency - value);
    });

    connect(okButton,&QPushButton::clicked,this,[=](bool)
    {
        gaussianHighPassFilter(maxFrequency - firstSlider->value());
        accept();
    });

    connect(firstLineEdit, &QLineEdit::textEdited,this,[=](QString text)
    {
        bool ok;
        int num = text.toInt(&ok);
        if(!ok || num < 0 || num > maxFrequency)
        {
            firstLineEdit->setText(QString::number(maxFrequency - firstSlider->value()));
        }
        else
        {
            firstSlider->setValue(maxFrequency - num);
        }
    });

    filterName->setText("Guassian high-pass filter");
    filterFormula->setText("G(ω) = 1 - exp(-ω²/2ω₀²)");

    firstPreferenceLabel->setVisible(true);
    firstPreferenceLabel->setText(QStringLiteral("ω₀: "));

    firstLineEdit->setVisible(true);
    firstLineEdit->setText(QString::number(maxFrequency - 1));

    firstFrontLabel->setVisible(true);
    firstFrontLabel->setText(QStringLiteral("0"));

    firstSlider->setVisible(true);
    firstSlider->setRange(0,maxFrequency);
    firstSlider->setValue(1);
    firstSlider->setInvertedAppearance(true);

    firstBackLabel->setVisible(true);
    firstBackLabel->setText(QString::number(maxFrequency));
}

void FilterDialog::initButterworthLowPass()
{
    connect(firstSlider,&QSlider::valueChanged, this, [=](int value)
    {
        firstLineEdit->setText(QString::number(value));
        blpfGraph(firstSlider->value(), spinBox->value());
    });

    connect(spinBox, static_cast<void (QSpinBox::*)(int i)> (&QSpinBox::valueChanged), this, [=](int value)
    {
        blpfGraph(firstSlider->value(), value);
    });

    connect(okButton,&QPushButton::clicked,this,[=](bool)
    {
        butterworthLowPassFilter(firstSlider->value(),spinBox->value());
        accept();
    });

    connect(firstLineEdit, &QLineEdit::textEdited,this,[=](QString text)
    {
        bool ok;
        int num = text.toInt(&ok);
        if(!ok || num < 0 || num > maxFrequency)
        {
            firstLineEdit->setText(QString::number(firstSlider->value()));
        }
        else
        {
            firstSlider->setValue(num);
        }
    });

    filterName->setText("Butterworth low-pass filter");
    filterFormula->setText("G(ω) = 1 / (1 + (ω/ω₀)²ⁿ)");

    firstPreferenceLabel->setVisible(true);
    firstPreferenceLabel->setText(QStringLiteral("ω₀: "));

    firstLineEdit->setVisible(true);
    firstLineEdit->setText(QString::number(maxFrequency - 1));

    firstFrontLabel->setVisible(true);
    firstFrontLabel->setText(QStringLiteral("0"));

    firstSlider->setVisible(true);
    firstSlider->setRange(0,maxFrequency);
    firstSlider->setValue(1);

    firstBackLabel->setVisible(true);
    firstBackLabel->setText(QString::number(maxFrequency));

    secondPreferenceLabel->setVisible(true);
    secondPreferenceLabel->setText(QStringLiteral("n: "));

    spinBox->setVisible(true);
    spinBox->setValue(2);
}

void FilterDialog::initButterworthHighPass()
{
    connect(firstSlider,&QSlider::valueChanged, this, [=](int value)
    {
        firstLineEdit->setText(QString::number(maxFrequency - value));
        bhpfGraph(maxFrequency - firstSlider->value(), spinBox->value());
    });

    connect(spinBox, static_cast<void (QSpinBox::*)(int i)> (&QSpinBox::valueChanged), this, [=](int value)
    {
        bhpfGraph(maxFrequency - firstSlider->value(), value);
    });

    connect(okButton,&QPushButton::clicked,this,[=](bool)
    {
        butterworthHighPassFilter(maxFrequency - firstSlider->value(),spinBox->value());
        accept();
    });

    connect(firstLineEdit, &QLineEdit::textEdited,this,[=](QString text)
    {
        bool ok;
        int num = text.toInt(&ok);
        if(!ok || num < 0 || num > maxFrequency)
        {
            firstLineEdit->setText(QString::number(maxFrequency - firstSlider->value()));
        }
        else
        {
            firstSlider->setValue(maxFrequency - num);
        }
    });

    filterName->setText("Butterworth high-pass filter");
    filterFormula->setText("G(ω) =  1 / (1 + (ω₀/ω)²ⁿ)");

    firstPreferenceLabel->setVisible(true);
    firstPreferenceLabel->setText(QStringLiteral("ω₀: "));

    firstLineEdit->setVisible(true);
    firstLineEdit->setText(QString::number(maxFrequency - 1));

    firstFrontLabel->setVisible(true);
    firstFrontLabel->setText(QStringLiteral("0"));

    firstSlider->setVisible(true);
    firstSlider->setRange(0,maxFrequency);
    firstSlider->setValue(1);
    firstSlider->setInvertedAppearance(true);

    firstBackLabel->setVisible(true);
    firstBackLabel->setText(QString::number(maxFrequency - 1));

    secondPreferenceLabel->setVisible(true);
    secondPreferenceLabel->setText(QStringLiteral("n: "));

    spinBox->setVisible(true);
    spinBox->setValue(2);
}


void FilterDialog::ilpfGraph(int c)
{
    plot->graph()->clearData();

    QVector<double> keys;
    QVector<double> values;

    for(int i = 0; i <= maxFrequency; i++)
    {
        keys.push_back(i);
        values.push_back(i <= c? 1 : 0);
    }
    keys.push_back(c+0.5);
    values.push_back(0);
    keys.push_back(c+0.5);
    values.push_back(1);

    plot->graph()->addData(keys,values);
    plot->replot();
}

void FilterDialog::ihpfGraph(int c)
{
    plot->graph()->clearData();

    QVector<double> keys;
    QVector<double> values;

    for(int i = 0; i <= maxFrequency; i++)
    {
        keys.push_back(i);
        values.push_back(i >= c? 1 : 0);
    }
    keys.push_back(c-0.5);
    values.push_back(1);
    keys.push_back(c-0.5);
    values.push_back(0);

    plot->graph()->addData(keys,values);
    plot->replot();
}

void FilterDialog::bpfGraph(int min, int max)
{
    plot->graph()->clearData();

    QVector<double> keys;
    QVector<double> values;

    for(int i = 0; i <= this->maxFrequency; i++)
    {
        keys.push_back(i);
        values.push_back(min <= i && i <= max ? 1 : 0);
    }
    keys.push_back(min-0.5);
    values.push_back(1);
    keys.push_back(min-0.5);
    values.push_back(0);

    keys.push_back(max + 0.5);
    values.push_back(0);
    keys.push_back(max + 0.5);
    values.push_back(1);

    plot->graph()->addData(keys,values);
    plot->replot();
}

void FilterDialog::glpfGraph(int omega0)
{
    plot->graph()->clearData();

    QVector<double> keys;
    QVector<double> values;

    //double norm = 1.0 / (sqrt(2*M_PI) * omega0);
    for(int i = 0; i <= this->maxFrequency; i++)
    {
        keys.push_back(i);

        if(omega0 == 0)
        {
            values.push_back(0);
        }
        else
        {
            //exp(-a.^2 / (2.0 * sigma^2));
            values.push_back(exp(- (double)(i * i) / ( 2.0 * omega0 * omega0)));
        }
    }


    plot->graph()->addData(keys,values);
    plot->replot();
}

void FilterDialog::ghpfGraph(int omega0)
{
    plot->graph()->clearData();

    QVector<double> keys;
    QVector<double> values;

    for(int i = 0; i <= this->maxFrequency; i++)
    {
        keys.push_back(i);
        if(omega0 == 0)
        {
            values.push_back(1);
        }
        else
        {
            values.push_back(1.0 - exp(- (double)(i * i) / (2.0 * omega0 * omega0)));
        }
    }

    plot->graph()->addData(keys,values);
    plot->replot();
}

void FilterDialog::blpfGraph(double omega0, int n)
{
    plot->graph()->clearData();

    QVector<double> keys;
    QVector<double> values;

    for(int i = 0; i <= this->maxFrequency; i++)
    {
        keys.push_back(i);
        if(omega0 == 0)
        {
            values.push_back(0);
        }
        else
        {
            values.push_back(1.0 / (1.0 + pow(((double)i / omega0), 2.0 * n)));
        }
    }

    plot->graph()->addData(keys,values);
    plot->replot();
}

void FilterDialog::bhpfGraph(double omega0, int n)
{
    plot->graph()->clearData();

    QVector<double> keys;
    QVector<double> values;

    for(int i = 0; i <= this->maxFrequency; i++)
    {
        keys.push_back(i);
        if(i == 0)
        {
            values.push_back(0);
        }
        else
        {
            values.push_back(1.0 / (1.0 + pow((omega0 / (double)i), 2.0 * n)));
        }
    }

    plot->graph()->addData(keys,values);
    plot->replot();
}

void FilterDialog::idealLowPassFilter(int value)
{
    QVector<double> x;
    QVector<double> y;

    int i = 0;
    int frekv = 0;


    while(i <= maxFrequency)
    {
        x.push_back(i);
        y.push_back(frekv <= value ? 1 : 0);
        i++;
        frekv++;
    }

    frekv--;

    if(magnitude.original_length() % 2 == 1)
    {
        x.push_back(i);
        y.push_back(frekv <= value ? 1 : 0);
        i++;
    }

    frekv--;

    while(i < magnitude.original_length())
    {
        x.push_back(i);
        y.push_back(frekv <= value ? 1 : 0);
        i++;
        frekv--;
    }

    Signal filter(x,y);

    // apply filter on magnitude
    magnitude = magnitude.applyFilter(filter);
}

void FilterDialog::idealHighPassFilter(int value)
{
    QVector<double> x;
    QVector<double> y;

    int i = 0;
    int frekv = 0;

    while(i <= maxFrequency)
    {
        x.push_back(i);
        y.push_back(frekv >= value ? 1 : 0);
        i++;
        frekv++;
    }

    frekv--;

    if(magnitude.original_length() % 2 == 1)
    {
        x.push_back(i);
        y.push_back(frekv >= value ? 1 : 0);
        i++;
    }

    frekv--;

    while(i < magnitude.original_length())
    {
        x.push_back(i);
        y.push_back(frekv >= value ? 1 : 0);
        i++;
        frekv--;
    }

    Signal filter(x,y);

    magnitude = magnitude.applyFilter(filter);
}

void FilterDialog::bandPassFilter(double min, double max)
{
    QVector<double> x;
    QVector<double> y;

    int i = 0;
    int frekv = 0;

    while(i <= max)
    {
        x.push_back(i);
        y.push_back(min <= frekv && frekv <= max ? 1 : 0);
        i++;
        frekv++;
    }

    frekv--;

    if(magnitude.original_length() % 2 == 1)
    {
        x.push_back(i);
        y.push_back(min <= frekv && frekv <= max ? 1 : 0);
        i++;
    }

    frekv--;

    while(i < magnitude.original_length())
    {
        x.push_back(i);
        y.push_back(min <= frekv && frekv <= max ? 1 : 0);
        i++;
        frekv--;
    }

    Signal filter(x,y);

    magnitude = magnitude.applyFilter(filter);
}

void FilterDialog::gaussianLowPassFilter(double omega0)
{
    QVector<double> x;
    QVector<double> y;

    int i = 0;
    int frekv = 0;

    while(i <= maxFrequency)
    {
        x.push_back(i);
        y.push_back(exp(- (double)(frekv * frekv) / ( 2.0 * omega0 * omega0)));
        i++;
        frekv++;
    }

    frekv--;

    if(magnitude.original_length() % 2 == 1)
    {
        x.push_back(i);
        y.push_back(exp(- (double)(frekv * frekv) / ( 2.0 * omega0 * omega0)));
        i++;
    }

    frekv--;

    while(i < magnitude.original_length())
    {
        x.push_back(i);
        y.push_back(exp(- (double)(frekv * frekv) / ( 2.0 * omega0 * omega0)));
        i++;
        frekv--;
    }

    Signal filter(x,y);

    magnitude = magnitude.applyFilter(filter);
}

void FilterDialog::gaussianHighPassFilter(double omega0)
{
    QVector<double> x;
    QVector<double> y;

    int i = 0;
    int frekv = 0;

    while(i <= maxFrequency)
    {
        x.push_back(i);
        y.push_back(omega0 == 0? 1.0 : 1.0 - exp(- (double)(frekv * frekv) / (2.0 * omega0 * omega0)));
        i++;
        frekv++;
    }

    frekv--;

    if(magnitude.original_length() % 2 == 1)
    {
        x.push_back(i);
        y.push_back(omega0 == 0? 1.0 : 1.0 - exp(- (double)(frekv * frekv) / (2.0 * omega0 * omega0)));
        i++;
    }

    frekv--;

    while(i < magnitude.original_length())
    {
        x.push_back(i);
        y.push_back(omega0 == 0? 1.0 : 1.0 - exp(- (double)(frekv * frekv) / (2.0 * omega0 * omega0)));
        i++;
        frekv--;
    }

    Signal filter(x,y);

    magnitude = magnitude.applyFilter(filter);
}

void FilterDialog::butterworthLowPassFilter(double omega0, int n)
{
    QVector<double> x;
    QVector<double> y;

    int i = 0;
    int frekv = 0;

    while(i <= maxFrequency)
    {
        x.push_back(i);
        y.push_back(omega0 == 0? 0 : 1.0 / (1.0 + pow(((double)frekv / omega0), 2.0 * n)));
        i++;
        frekv++;
    }

    frekv--;

    if(magnitude.original_length() % 2 == 1)
    {
        x.push_back(i);
        y.push_back(omega0 == 0? 0 : 1.0 / (1.0 + pow(((double)frekv / omega0), 2.0 * n)));
        i++;
    }

    frekv--;

    while(i < magnitude.original_length())
    {
        x.push_back(i);
        y.push_back(omega0 == 0? 0 : 1.0 / (1.0 + pow(((double)frekv / omega0), 2.0 * n)));
        i++;
        frekv--;
    }

    Signal filter(x,y);

    magnitude = magnitude.applyFilter(filter);
}

void FilterDialog::butterworthHighPassFilter(double omega0, int n)
{
    QVector<double> x;
    QVector<double> y;

    x.push_back(0);
    y.push_back(0);

    int i = 1;
    int frekv = 1;

    while(i <= maxFrequency)
    {
        x.push_back(i);
        y.push_back(1.0 / (1.0 + pow((omega0 / (double)frekv), 2.0 * n)));
        i++;
        frekv++;
    }

    frekv--;

    if(magnitude.original_length() % 2 == 1)
    {
        x.push_back(i);
        y.push_back(1.0 / (1.0 + pow((omega0 / (double)frekv), 2.0 * n)));
        i++;
    }

    frekv--;

    while(i < magnitude.original_length())
    {
        x.push_back(i);
        y.push_back(1.0 / (1.0 + pow((omega0 / (double)frekv), 2.0 * n)));
        i++;
        frekv--;
    }

    Signal filter(x,y);

    magnitude = magnitude.applyFilter(filter);
}


FilterDialog::~FilterDialog()
{
    delete filterName;
    delete filterFormula;

    delete firstPreferenceLabel;
    delete secondPreferenceLabel;

    delete firstLineEdit;
    delete secondLineEdit;

    delete firstSlider;
    delete secondSlider;

    delete firstFrontLabel;
    delete firstBackLabel;

    delete secondFrontLabel;
    delete secondBackLabel;

    delete okButton;
    delete cancelButton;

    delete plot;

    delete spinBox;
}

void FilterDialog::setDefaultTexts(FilterType type)
{
    setWindowTitle(QStringLiteral("Setup filter properties..."));
    switch (type)
    {
    case ILPF:
        filterName->setText(QStringLiteral("Ideal low-pass filter"));
        break;
    case IHPF:
        filterName->setText(QStringLiteral("Ideal high-pass filter"));
        break;
    case BANDPASS:
        filterName->setText(QStringLiteral("Idea band-pass filter"));
        break;
    case LPGAUSS:
        filterName->setText(QStringLiteral("Gaussian low-pass filter"));
        break;
    case HPGAUSS:
        filterName->setText(QStringLiteral("Gaussian high-pass filter"));
        break;
    case LPBUTTERWORTH:
        filterName->setText(QStringLiteral("Butterworth low-pass filter"));
        break;
    case HPBUTTERWORTH:
        filterName->setText(QStringLiteral("Butterworth high-pass filter"));
        break;
    default:
        break;
    }
}


void FilterDialog::setLocalizedTexts(const Translation* language, FilterType type)
{
    if(language == nullptr)
    {
        setDefaultTexts(type);
    }
    else
    {

        setWindowTitle(language->getTitle());
        if(windowTitle().isEmpty()) setWindowTitle(QStringLiteral("About FTutor1D"));

        okButton->setText(language->getChildElementText(QStringLiteral("okButton")));
        if(okButton->text().isEmpty()) okButton->setText(QStringLiteral("OK"));

        cancelButton->setText(language->getChildElementText(QStringLiteral("cancelButton")));
        if(cancelButton->text().isEmpty()) cancelButton->setText(QStringLiteral("Cancel"));

        Translation* tr = nullptr;

        switch (type)
        {
        case ILPF:
            tr = language->getTranslationForUseCase(QStringLiteral("IdealLowPass"));
            if(tr) filterName->setText(tr->getChildElementText(QStringLiteral("filterName")));
            if(filterName->text().isEmpty()) filterName->setText(QStringLiteral("Ideal low-pass filter"));
            break;
        case IHPF:
            tr = language->getTranslationForUseCase(QStringLiteral("IdealHighPass"));
            if(tr) filterName->setText(tr->getChildElementText(QStringLiteral("filterName")));
            if(filterName->text().isEmpty()) filterName->setText(QStringLiteral("Ideal high-pass filter"));
            break;
        case BANDPASS:
            tr = language->getTranslationForUseCase(QStringLiteral("BandPass"));
            if(tr) filterName->setText(tr->getChildElementText(QStringLiteral("filterName")));
            if(filterName->text().isEmpty()) filterName->setText(QStringLiteral("Ideal band-pass filter"));
            break;
        case LPGAUSS:
            tr = language->getTranslationForUseCase(QStringLiteral("GaussianLowPass"));
            if(tr) filterName->setText(tr->getChildElementText(QStringLiteral("filterName")));
            if(filterName->text().isEmpty()) filterName->setText(QStringLiteral("Gaussian low-pass filter"));
            break;
        case HPGAUSS:
            tr = language->getTranslationForUseCase(QStringLiteral("GaussianHighPass"));
            if(tr) filterName->setText(tr->getChildElementText(QStringLiteral("filterName")));
            if(filterName->text().isEmpty()) filterName->setText(QStringLiteral("Gaussian high-pass filter"));
            break;
        case LPBUTTERWORTH:
            tr = language->getTranslationForUseCase(QStringLiteral("ButterworthLowPass"));
            if(tr) filterName->setText(tr->getChildElementText(QStringLiteral("filterName")));
            if(filterName->text().isEmpty()) filterName->setText(QStringLiteral("Butterworth low-pass filter"));
            break;
        case HPBUTTERWORTH:
            tr = language->getTranslationForUseCase(QStringLiteral("ButterworthHighPass"));
            if(tr) filterName->setText(tr->getChildElementText(QStringLiteral("filterName")));
            if(filterName->text().isEmpty()) filterName->setText(QStringLiteral("Butterworth high-pass filter"));
            break;
        default:
            break;
        }

        delete tr;
    }
}
