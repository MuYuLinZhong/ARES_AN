import { Search, PlusCircle, DoorOpen, DoorClosed, Warehouse, Building2, LockOpen } from 'lucide-react';
import { View } from '../App';
import BottomNav from '../components/BottomNav';

const DEVICES = [
  { id: '882-991-002', name: 'Office Front Door', icon: DoorOpen, color: 'text-primary', bg: 'bg-primary/10' },
  { id: '112-445-901', name: 'Home Side Gate', icon: DoorClosed, color: 'text-slate-400', bg: 'bg-slate-800' },
  { id: '304-221-550', name: 'South Garage', icon: Warehouse, color: 'text-slate-400', bg: 'bg-slate-800' },
  { id: '990-112-443', name: 'Main Lobby', icon: Building2, color: 'text-primary', bg: 'bg-primary/10' },
];

export default function Devices({ onNavigate }: { onNavigate: (view: View, data?: any) => void }) {
  return (
    <div className="flex-1 flex flex-col h-screen overflow-hidden">
      <header className="pt-6 px-4 pb-2 border-b border-slate-800 bg-background-dark sticky top-0 z-10">
        <div className="grid grid-cols-3 items-center mb-4">
          <div></div>
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
            placeholder="Search by nickname" 
            className="block w-full pl-10 pr-4 py-2.5 bg-slate-800/50 border-none rounded-xl focus:ring-2 focus:ring-primary text-sm placeholder:text-slate-400 text-white outline-none"
          />
        </div>
      </header>

      <main className="flex-1 overflow-y-auto hide-scrollbar px-4 py-4 space-y-4 pb-32">
        {DEVICES.map(device => (
          <div key={device.id} className="bg-slate-900/40 border border-slate-800 rounded-xl p-4 shadow-sm">
            <div className="flex gap-4 items-center">
              <div className={`size-16 rounded-lg ${device.bg} flex items-center justify-center shrink-0`}>
                <device.icon size={32} className={device.color} />
              </div>
              <div className="flex-1 min-w-0 flex flex-col justify-center">
                <h3 className="text-base font-bold text-slate-100 truncate">{device.name}</h3>
                <p className="text-xs text-slate-400 mt-1">ID: {device.id}</p>
              </div>
            </div>
            <div className="mt-4 flex gap-2">
              <button className="flex-1 py-2 bg-primary text-white rounded-lg text-sm font-semibold flex items-center justify-center gap-2">
                <LockOpen size={16} /> Unlock
              </button>
              <button 
                onClick={() => onNavigate('device-details', { deviceId: device.id })}
                className="px-4 py-2 bg-slate-800 text-slate-300 rounded-lg text-sm font-semibold hover:bg-slate-700 transition-colors"
              >
                Manage
              </button>
            </div>
          </div>
        ))}
      </main>

      <BottomNav currentView="devices" onNavigate={onNavigate} />
    </div>
  );
}
