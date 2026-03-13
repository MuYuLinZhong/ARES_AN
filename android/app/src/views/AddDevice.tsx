import { useState, useEffect, useCallback } from 'react';
import { Nfc, LockOpen, CheckCircle, XCircle, ArrowLeft } from 'lucide-react';
import { View } from '../App';
import BottomNav from '../components/BottomNav';

declare global {
  interface Window {
    AndroidBridge?: {
      onStartNfcScan(json: string): void;
      onCancelNfcScan(): void;
    };
    onNfcScanStateChanged?: (payload: { state: string; deviceId?: string; message?: string }) => void;
  }
}

type ScanPhase = 'idle' | 'scanning' | 'connecting' | 'success' | 'error';

export default function AddDevice({ onNavigate }: { onNavigate: (view: View) => void }) {
  const [nickname, setNickname] = useState('');
  const [scanPhase, setScanPhase] = useState<ScanPhase>('idle');
  const [scanMessage, setScanMessage] = useState('');
  const [addedDeviceId, setAddedDeviceId] = useState('');

  useEffect(() => {
    window.onNfcScanStateChanged = (payload) => {
      setScanPhase(payload.state as ScanPhase);
      if (payload.state === 'success' && payload.deviceId) {
        setAddedDeviceId(payload.deviceId);
      }
      if (payload.state === 'error' && payload.message) {
        setScanMessage(payload.message);
      }
    };
    return () => { window.onNfcScanStateChanged = undefined; };
  }, []);

  const handleStartScan = useCallback(() => {
    if (!nickname.trim()) return;
    setScanPhase('scanning');
    setScanMessage('');
    window.AndroidBridge?.onStartNfcScan(JSON.stringify({ nickname: nickname.trim() }));
  }, [nickname]);

  const handleCancel = useCallback(() => {
    window.AndroidBridge?.onCancelNfcScan();
    setScanPhase('idle');
  }, []);

  return (
    <div className="flex-1 flex flex-col h-screen overflow-hidden">
      <header className="flex items-center px-4 py-6">
        <button onClick={() => onNavigate('devices')} className="size-10 flex items-center justify-center text-slate-300">
          <ArrowLeft size={24} />
        </button>
        <h1 className="text-lg font-bold tracking-tight text-slate-100 flex-1 text-center pr-10">Add New Device</h1>
      </header>

      <main className="flex-1 flex flex-col px-6">
        <div className="mt-4 space-y-2">
          <label className="text-sm font-semibold text-slate-400 ml-1" htmlFor="device-nickname">
            Device Nickname
          </label>
          <input
            id="device-nickname"
            type="text"
            value={nickname}
            onChange={e => setNickname(e.target.value)}
            placeholder="Enter device nickname (e.g., Office)"
            disabled={scanPhase !== 'idle'}
            className="w-full bg-slate-800/50 border-none rounded-xl h-14 px-4 text-base focus:ring-2 focus:ring-primary placeholder:text-slate-600 text-white outline-none transition-all disabled:opacity-50"
          />
        </div>

        <div className="mt-8">
          {scanPhase === 'idle' && (
            <button
              onClick={handleStartScan}
              disabled={!nickname.trim()}
              className="w-full bg-primary hover:bg-primary/90 text-white h-16 rounded-xl font-bold text-lg shadow-lg shadow-primary/20 transition-all flex items-center justify-center gap-3 disabled:opacity-40 disabled:cursor-not-allowed"
            >
              <Nfc size={24} />
              Approach and Add
            </button>
          )}
          {(scanPhase === 'scanning' || scanPhase === 'connecting') && (
            <button
              onClick={handleCancel}
              className="w-full bg-slate-800 text-slate-300 h-16 rounded-xl font-bold text-lg transition-all flex items-center justify-center gap-3"
            >
              Cancel Scan
            </button>
          )}
        </div>

        <div className="flex-1 flex flex-col items-center justify-center py-12">
          {scanPhase === 'idle' && (
            <>
              <div className="relative w-64 h-64 flex items-center justify-center rounded-full" style={{ background: 'radial-gradient(circle at center, rgba(19, 127, 236, 0.15) 0%, transparent 70%)' }}>
                <div className="relative z-10 w-32 h-56 border-4 border-slate-700 rounded-[2.5rem] bg-slate-900 p-1">
                  <div className="w-full h-full rounded-[2.2rem] bg-background-dark overflow-hidden flex flex-col items-center pt-4">
                    <div className="w-12 h-1 rounded-full bg-slate-700 mb-8" />
                    <Nfc size={48} className="text-primary opacity-50" />
                  </div>
                </div>
                <div className="absolute top-0 right-4 w-20 h-20 bg-slate-800 rounded-2xl shadow-xl flex items-center justify-center border border-slate-700">
                  <LockOpen size={32} className="text-slate-400" />
                </div>
                <div className="absolute top-10 right-10 size-16 rounded-full border-2 border-primary/30" />
                <div className="absolute top-6 right-6 size-24 rounded-full border-2 border-primary/10" />
              </div>
              <div className="mt-8 text-center space-y-2">
                <h3 className="text-base font-bold text-slate-100">Hold phone near the lock</h3>
                <p className="text-sm text-slate-400 max-w-[240px] mx-auto">
                  Enter a nickname, then tap "Approach and Add" and hold your phone near the NFC lock.
                </p>
              </div>
            </>
          )}

          {(scanPhase === 'scanning' || scanPhase === 'connecting') && (
            <div className="flex flex-col items-center">
              <div className="w-20 h-20 rounded-full bg-primary/20 flex items-center justify-center mb-6 animate-pulse">
                <Nfc size={40} className="text-primary" />
              </div>
              <h3 className="text-lg font-bold text-slate-100 mb-2">
                {scanPhase === 'scanning' ? 'Scanning...' : 'Connecting...'}
              </h3>
              <p className="text-sm text-slate-400">Hold phone near the NFC lock</p>
            </div>
          )}

          {scanPhase === 'success' && (
            <div className="flex flex-col items-center">
              <CheckCircle size={64} className="text-green-400 mb-6" />
              <h3 className="text-lg font-bold text-slate-100 mb-2">Device Added</h3>
              <p className="text-sm text-slate-400 mb-1">"{nickname}" has been registered</p>
              <p className="text-xs text-slate-500">ID: {addedDeviceId}</p>
              <button
                onClick={() => onNavigate('devices')}
                className="mt-8 px-8 py-3 bg-primary rounded-full text-sm font-bold text-white"
              >
                Go to Devices
              </button>
            </div>
          )}

          {scanPhase === 'error' && (
            <div className="flex flex-col items-center">
              <XCircle size={64} className="text-red-400 mb-6" />
              <h3 className="text-lg font-bold text-slate-100 mb-2">Failed</h3>
              <p className="text-sm text-slate-400 text-center max-w-[240px]">{scanMessage}</p>
              <button
                onClick={() => setScanPhase('idle')}
                className="mt-8 px-8 py-3 bg-slate-800 rounded-full text-sm font-bold text-slate-300"
              >
                Try Again
              </button>
            </div>
          )}
        </div>
      </main>

      <BottomNav currentView="add-device" onNavigate={onNavigate} />
    </div>
  );
}
