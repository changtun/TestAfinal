/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: /home/klin/workspace/DVB_AMLOGIC_SX_SVN/src/com/pbi/dvb/aidl/PiwikTrackerManagerServiceAIDL.aidl
 */
package com.pbi.dvb.aidl;
public interface PiwikTrackerManagerServiceAIDL extends android.os.IInterface
{
/** Local-side IPC implementation stub class. */
public static abstract class Stub extends android.os.Binder implements com.pbi.dvb.aidl.PiwikTrackerManagerServiceAIDL
{
private static final java.lang.String DESCRIPTOR = "com.pbi.dvb.aidl.PiwikTrackerManagerServiceAIDL";
/** Construct the stub at attach it to the interface. */
public Stub()
{
this.attachInterface(this, DESCRIPTOR);
}
/**
 * Cast an IBinder object into an com.pbi.dvb.aidl.PiwikTrackerManagerServiceAIDL interface,
 * generating a proxy if needed.
 */
public static com.pbi.dvb.aidl.PiwikTrackerManagerServiceAIDL asInterface(android.os.IBinder obj)
{
if ((obj==null)) {
return null;
}
android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
if (((iin!=null)&&(iin instanceof com.pbi.dvb.aidl.PiwikTrackerManagerServiceAIDL))) {
return ((com.pbi.dvb.aidl.PiwikTrackerManagerServiceAIDL)iin);
}
return new com.pbi.dvb.aidl.PiwikTrackerManagerServiceAIDL.Stub.Proxy(obj);
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
case TRANSACTION_sendSignal:
{
data.enforceInterface(DESCRIPTOR);
this.sendSignal();
reply.writeNoException();
return true;
}
}
return super.onTransact(code, data, reply, flags);
}
private static class Proxy implements com.pbi.dvb.aidl.PiwikTrackerManagerServiceAIDL
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
@Override public void sendSignal() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_sendSignal, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
}
static final int TRANSACTION_sendSignal = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
}
public void sendSignal() throws android.os.RemoteException;
}
