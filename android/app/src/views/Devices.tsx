import { useState, useEffect, useCallback } from 'react';
import { Search, PlusCircle, DoorOpen, LockOpen } from 'lucide-react';
import { View } from '../App';
import BottomNav from '../components/BottomNav';

declare global {
  interface Window {
    AndroidBridge?: {
      onRefreshDeviceList(): void;
      onSearchKeywordChanged(json: string): void;
      onUnlockFromList(json: string): void;
    };
    onDevicesLoaded?: (payload: { devices: DeviceData[]; isOffline: boolean }) => void;
  }
}

interface DeviceData {
  deviceId: string;
  nickname: string;
  serialNo: string;
  isValid: boolean;
  lastSyncAt: number;
}

export default function Devices({ onNavigate }: { onNavigate: (view: View, data?: any) => void }) {
  const [devices, setDevices] = useState<DeviceData[]>([]);
  const [keyword, setKeyword] = useState('');
  const [isOffline, setIsOffline] = useState(false);

  useEffect(() => {
    window.onDevicesLoaded = (payload) => {
      setDevices(payload.devices || []);
      setIsOffline(payload.isOffline);
    };
    window.AndroidBridge?.onRefreshDeviceList();
    return () => { window.onDevicesLoaded = undefined; };
  }, []);

  const handleSearch = useCallback((value: string) => {
    setKeyword(value);
    window.AndroidBridge?.onSearchKeywordChanged(JSON.stringify({ keyword: value }));
  }, []);

  const handleUnlock = useCallback((device: DeviceData) => {
    window.AndroidBridge?.onUnlockFromList(
      JSON.stringify({ deviceId: device.deviceId, deviceName: device.nickname })
    );
    onNavigate('home');
  }, [onNavigate]);

  return (
    <div className="flex-1 flex flex-col h-screen overflow-hidden">
      <header className="pt-6 px-4 pb-2 border-b border-slate-800 bg-background-dark sticky top-0 z-10">
        <div className="grid grid-cols-3 items-center mb-4">
          <div />
          <h1 className="text-xl font-bold tracking-tight text-center whitespace-nowrap">My Devices</h1>
          <div className="flex items-center justify-end">
            <button onClick={() => onNavigate('add-device')} className="p-2 -mr-2 text-primary">
              <PlusCircle size={24} />
            </button>
          </div>
        </div>

        <div className="relative mb-2">
          <div className="absolute inset-y-0 left-0 pl-3 flex items-center pointer-events-none text-slate-400">
            <Search size={20} />
          </div>
          <input
            type="text"
            value={keyword}
            onChange={e => handleSearch(e.target.value)}
            placeholder="Search by nickname"
            className="block w-full pl-10 pr-4 py-2.5 bg-slate-800/50 border-none rounded-xl focus:ring-2 focus:ring-primary text-sm placeholder:text-slate-400 text-white outline-none"
          />
        </div>
      </header>

      {isOffline && (
        <div className="px-4 pt-2">
          <div className="bg-yellow-500/10 border border-yellow-500/20 rounded-lg px-3 py-2 text-xs text-yellow-400 text-center">
            Offline — showing cached devices
          </div>
        </div>
      )}

      <main className="flex-1 overflow-y-auto hide-scrollbar px-4 py-4 space-y-4 pb-32">
        {devices.length === 0 ? (
          <div className="flex flex-col items-center justify-center py-20 text-slate-500">
            <DoorOpen size={48} className="mb-4 opacity-50" />
            <p className="text-sm font-medium">No devices found</p>
            <p className="text-xs mt-1">Tap + to add a new device via NFC</p>
          </div>
        ) : (
          devices.map(device => (
            <div key={device.deviceId} className="bg-slate-900/40 border border-slate-800 rounded-xl p-4 shadow-sm">
              <div className="flex gap-4 items-center">
                <div className={`size-16 rounded-lg flex items-center justify-center shrink-0 ${device.isValid ? 'bg-primary/10' : 'bg-slate-800'}`}>
                  <DoorOpen size={32} className={device.isValid ? 'text-primary' : 'text-slate-500'} />
                </div>
                <div className="flex-1 min-w-0 flex flex-col justify-center">
                  <h3 className="text-base font-bold text-slate-100 truncate">{device.nickname}</h3>
                  <p className="text-xs text-slate-400 mt-1">ID: {device.deviceId}</p>
                </div>
              </div>
              <div className="mt-4 flex gap-2">
                <button
                  onClick={() => handleUnlock(device)}
                  disabled={!device.isValid}
                  className="flex-1 py-2 bg-primary text-white rounded-lg text-sm font-semibold flex items-center justify-center gap-2 disabled:opacity-40 disabled:cursor-not-allowed"
                >
                  <LockOpen size={16} /> Unlock
                </button>
                <button
                  onClick={() => onNavigate('device-details', { deviceId: device.deviceId })}
                  className="px-4 py-2 bg-slate-800 text-slate-300 rounded-lg text-sm font-semibold hover:bg-slate-700 transition-colors"
                >
                  Manage
                </button>
              </div>
            </div>
          ))
        )}
      </main>

      <BottomNav currentView="devices" onNavigate={onNavigate} />
    </div>
  );
}
