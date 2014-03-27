/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: /home/klin/DVB_WORKSPACE/DVB_SX_1219/src/com/pbi/dvb/aidl/PvrRecordServiceAIDL.aidl
 */
package com.pbi.dvb.aidl;
public interface PvrRecordServiceAIDL extends android.os.IInterface
{
/** Local-side IPC implementation stub class. */
public static abstract class Stub extends android.os.Binder implements com.pbi.dvb.aidl.PvrRecordServiceAIDL
{
private static final java.lang.String DESCRIPTOR = "com.pbi.dvb.aidl.PvrRecordServiceAIDL";
/** Construct the stub at attach it to the interface. */
public Stub()
{
this.attachInterface(this, DESCRIPTOR);
}
/**
 * Cast an IBinder object into an com.pbi.dvb.aidl.PvrRecordServiceAIDL interface,
 * generating a proxy if needed.
 */
public static com.pbi.dvb.aidl.PvrRecordServiceAIDL asInterface(android.os.IBinder obj)
{
if ((obj==null)) {
return null;
}
android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
if (((iin!=null)&&(iin instanceof com.pbi.dvb.aidl.PvrRecordServiceAIDL))) {
return ((com.pbi.dvb.aidl.PvrRecordServiceAIDL)iin);
}
return new com.pbi.dvb.aidl.PvrRecordServiceAIDL.Stub.Proxy(obj);
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
case TRANSACTION_startRecord:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
this.startRecord(_arg0);
reply.writeNoException();
return true;
}
case TRANSACTION_stopRecord:
{
data.enforceInterface(DESCRIPTOR);
this.stopRecord();
reply.writeNoException();
return true;
}
case TRANSACTION_getCurrentTunerFreq:
{
data.enforceInterface(DESCRIPTOR);
int _result = this.getCurrentTunerFreq();
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_getCurrentTunerSymbol:
{
data.enforceInterface(DESCRIPTOR);
int _result = this.getCurrentTunerSymbol();
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_getCurrentTunerQAM:
{
data.enforceInterface(DESCRIPTOR);
int _result = this.getCurrentTunerQAM();
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_getTunerInfo:
{
data.enforceInterface(DESCRIPTOR);
int[] _result = this.getTunerInfo();
reply.writeNoException();
reply.writeIntArray(_result);
return true;
}
}
return super.onTransact(code, data, reply, flags);
}
private static class Proxy implements com.pbi.dvb.aidl.PvrRecordServiceAIDL
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
@Override public void startRecord(int pvrID) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(pvrID);
mRemote.transact(Stub.TRANSACTION_startRecord, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
@Override public void stopRecord() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_stopRecord, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
@Override public int getCurrentTunerFreq() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getCurrentTunerFreq, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public int getCurrentTunerSymbol() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getCurrentTunerSymbol, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public int getCurrentTunerQAM() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getCurrentTunerQAM, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public int[] getTunerInfo() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int[] _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getTunerInfo, _data, _reply, 0);
_reply.readException();
_result = _reply.createIntArray();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
}
static final int TRANSACTION_startRecord = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
static final int TRANSACTION_stopRecord = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
static final int TRANSACTION_getCurrentTunerFreq = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
static final int TRANSACTION_getCurrentTunerSymbol = (android.os.IBinder.FIRST_CALL_TRANSACTION + 3);
static final int TRANSACTION_getCurrentTunerQAM = (android.os.IBinder.FIRST_CALL_TRANSACTION + 4);
static final int TRANSACTION_getTunerInfo = (android.os.IBinder.FIRST_CALL_TRANSACTION + 5);
}
public void startRecord(int pvrID) throws android.os.RemoteException;
public void stopRecord() throws android.os.RemoteException;
public int getCurrentTunerFreq() throws android.os.RemoteException;
public int getCurrentTunerSymbol() throws android.os.RemoteException;
public int getCurrentTunerQAM() throws android.os.RemoteException;
public int[] getTunerInfo() throws android.os.RemoteException;
}
