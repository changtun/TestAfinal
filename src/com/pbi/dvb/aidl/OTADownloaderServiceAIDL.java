/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: /home/klin/workspace/DVB_AMLOGIC_SX_SVN/src/com/pbi/dvb/aidl/OTADownloaderServiceAIDL.aidl
 */
package com.pbi.dvb.aidl;
public interface OTADownloaderServiceAIDL extends android.os.IInterface
{
/** Local-side IPC implementation stub class. */
public static abstract class Stub extends android.os.Binder implements com.pbi.dvb.aidl.OTADownloaderServiceAIDL
{
private static final java.lang.String DESCRIPTOR = "com.pbi.dvb.aidl.OTADownloaderServiceAIDL";
/** Construct the stub at attach it to the interface. */
public Stub()
{
this.attachInterface(this, DESCRIPTOR);
}
/**
 * Cast an IBinder object into an com.pbi.dvb.aidl.OTADownloaderServiceAIDL interface,
 * generating a proxy if needed.
 */
public static com.pbi.dvb.aidl.OTADownloaderServiceAIDL asInterface(android.os.IBinder obj)
{
if ((obj==null)) {
return null;
}
android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
if (((iin!=null)&&(iin instanceof com.pbi.dvb.aidl.OTADownloaderServiceAIDL))) {
return ((com.pbi.dvb.aidl.OTADownloaderServiceAIDL)iin);
}
return new com.pbi.dvb.aidl.OTADownloaderServiceAIDL.Stub.Proxy(obj);
}
@Override public android.os.IBinder asBinder()
{
return this;
}
@Override public boolean onTransact(int code, android.os.Parcel data, android.os.Parcel reply, int flags) throws android.os.RemoteException
{
switch (code)
{
case INTERFACE_TRANSACTION:
{
reply.writeString(DESCRIPTOR);
return true;
}
case TRANSACTION_try_Lock_OTA_IP:
{
data.enforceInterface(DESCRIPTOR);
boolean _result = this.try_Lock_OTA_IP();
reply.writeNoException();
reply.writeInt(((_result)?(1):(0)));
return true;
}
case TRANSACTION_try_Lock_OTA:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
boolean _result = this.try_Lock_OTA(_arg0);
reply.writeNoException();
reply.writeInt(((_result)?(1):(0)));
return true;
}
case TRANSACTION_unlock_OTA_IP:
{
data.enforceInterface(DESCRIPTOR);
boolean _result = this.unlock_OTA_IP();
reply.writeNoException();
reply.writeInt(((_result)?(1):(0)));
return true;
}
case TRANSACTION_unlock_OTA:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
boolean _result = this.unlock_OTA(_arg0);
reply.writeNoException();
reply.writeInt(((_result)?(1):(0)));
return true;
}
}
return super.onTransact(code, data, reply, flags);
}
private static class Proxy implements com.pbi.dvb.aidl.OTADownloaderServiceAIDL
{
private android.os.IBinder mRemote;
Proxy(android.os.IBinder remote)
{
mRemote = remote;
}
@Override public android.os.IBinder asBinder()
{
return mRemote;
}
public java.lang.String getInterfaceDescriptor()
{
return DESCRIPTOR;
}
@Override public boolean try_Lock_OTA_IP() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
boolean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_try_Lock_OTA_IP, _data, _reply, 0);
_reply.readException();
_result = (0!=_reply.readInt());
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public boolean try_Lock_OTA(int lockMask) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
boolean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(lockMask);
mRemote.transact(Stub.TRANSACTION_try_Lock_OTA, _data, _reply, 0);
_reply.readException();
_result = (0!=_reply.readInt());
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public boolean unlock_OTA_IP() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
boolean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_unlock_OTA_IP, _data, _reply, 0);
_reply.readException();
_result = (0!=_reply.readInt());
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public boolean unlock_OTA(int lockMask) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
boolean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(lockMask);
mRemote.transact(Stub.TRANSACTION_unlock_OTA, _data, _reply, 0);
_reply.readException();
_result = (0!=_reply.readInt());
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
}
static final int TRANSACTION_try_Lock_OTA_IP = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
static final int TRANSACTION_try_Lock_OTA = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
static final int TRANSACTION_unlock_OTA_IP = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
static final int TRANSACTION_unlock_OTA = (android.os.IBinder.FIRST_CALL_TRANSACTION + 3);
}
public boolean try_Lock_OTA_IP() throws android.os.RemoteException;
public boolean try_Lock_OTA(int lockMask) throws android.os.RemoteException;
public boolean unlock_OTA_IP() throws android.os.RemoteException;
public boolean unlock_OTA(int lockMask) throws android.os.RemoteException;
}
