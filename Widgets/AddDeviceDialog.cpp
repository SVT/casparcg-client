#include "AddDeviceDialog.h"

#include "DatabaseManager.h"

#include <QtCore/QDebug>

#include <QtGui/QAbstractButton>
#include <QtGui/QMessageBox>

AddDeviceDialog::AddDeviceDialog(QWidget* parent)
    : QDialog(parent), isClosing(false)
{
    setupUi(this);
}

void AddDeviceDialog::closeEvent(QCloseEvent* event)
{
    this->isClosing = true;
    QDialog::closeEvent(event);
}

void AddDeviceDialog::lookupName()
{
    if (this->isClosing)
        return;

    DeviceModel model = DatabaseManager::getInstance().getDeviceByName(this->lineEditDeviceName->text());
    if (!model.getName().isEmpty())
    {
        QMessageBox box;
        box.setWindowTitle("Add device");
        box.setText("The name already exists in the database. Please choose a unique name for the device.");
        box.setIcon(QMessageBox::Critical);
        box.setStandardButtons(QMessageBox::Ok);
        box.buttons().at(0)->setFocusPolicy(Qt::NoFocus);
        box.exec();

        this->lineEditDeviceName->setFocus();
        this->lineEditDeviceName->selectAll();
    }
}

void AddDeviceDialog::lookupAddress()
{
    if (this->isClosing)
        return;

    DeviceModel model = DatabaseManager::getInstance().getDeviceByAddress(this->lineEditAddress->text());
    if (!model.getName().isEmpty())
    {
        QMessageBox box;
        box.setWindowTitle("Add device");
        box.setText("The address already exists in the database. Please choose a unique address for the device.");
        box.setIcon(QMessageBox::Critical);
        box.setStandardButtons(QMessageBox::Ok);
        box.buttons().at(0)->setFocusPolicy(Qt::NoFocus);
        box.exec();

        this->lineEditAddress->setFocus();
        this->lineEditAddress->selectAll();

        return;
    }

    this->device = QSharedPointer<CasparDevice>(new CasparDevice());

    QObject::connect(this->device.data(), SIGNAL(connectionStateChanged(CasparDevice&)), this, SLOT(deviceConnectionStateChanged(CasparDevice&)));
    QObject::connect(this->device.data(), SIGNAL(versionChanged(const CasparVersion&, CasparDevice&)), this, SLOT(deviceServerVersionChanged(const CasparVersion&, CasparDevice&)));
    QObject::connect(this->device.data(), SIGNAL(infoChanged(const QList<QString>&, CasparDevice&)), this, SLOT(deviceInfoChanged(const QList<QString>&, CasparDevice&)));

    if (this->lineEditPort->text().isEmpty())
        this->device->connect(this->lineEditAddress->text());
    else
        this->device->connect(this->lineEditAddress->text(), this->lineEditPort->text().toInt());
}

const QString AddDeviceDialog::getName() const
{
    return this->lineEditDeviceName->text();
}

const QString AddDeviceDialog::getAddress() const
{
    return this->lineEditAddress->text();
}

const QString AddDeviceDialog::getPort() const
{
    if (!this->lineEditPort->text().isEmpty())
        return this->lineEditPort->text();
    else
        return this->lineEditPort->placeholderText();
}

const QString AddDeviceDialog::getUsername() const
{
    return this->lineEditUsername->text();
}

const QString AddDeviceDialog::getPassword() const
{
    return this->lineEditPassword->text();
}

const QString AddDeviceDialog::getDescription() const
{
    return this->lineEditDescription->text();
}

const QString AddDeviceDialog::getVersion() const
{
    if (this->lineEditVersion->text().isEmpty())
        return this->lineEditVersion->placeholderText();

    return this->lineEditVersion->text();
}

int AddDeviceDialog::getChannels() const
{
    if (this->lineEditChannels->text().isEmpty())
        return 0;

    return this->lineEditChannels->text().toInt();
}

const QString AddDeviceDialog::getShadow() const
{
    return this->checkBoxShadow->checkState() == Qt::Checked ? "Yes" : "No";
}

void AddDeviceDialog::deviceConnectionStateChanged(CasparDevice& device)
{
    if (this->device->isConnected())
        this->device->refreshServerVersion();
}

void AddDeviceDialog::deviceServerVersionChanged(const CasparVersion& version, CasparDevice& device)
{
    this->lineEditVersion->setText(version.getVersion());

    if (this->device->isConnected())
        this->device->refreshChannels();
}

void AddDeviceDialog::deviceInfoChanged(const QList<QString>& info, CasparDevice& device)
{
    this->lineEditChannels->setText(QString("%1").arg(info.count()));

    this->pushButtonOK->setEnabled(true);
    this->device->disconnect();
}
