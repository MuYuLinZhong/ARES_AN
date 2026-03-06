import { Home, Smartphone, PlusCircle } from 'lucide-react';
import { View } from '../App';

interface BottomNavProps {
  currentView: View;
  onNavigate: (view: View) => void;
}

export default function BottomNav({ currentView, onNavigate }: BottomNavProps) {
  return (
    <nav className="fixed bottom-0 w-full max-w-md bg-[#15202c] border-t border-slate-800 flex justify-around items-center pt-3 pb-8 px-6 z-30">
      <button 
        onClick={() => onNavigate('home')}
        className={`flex flex-col items-center gap-1 p-2 transition-colors ${currentView === 'home' ? 'text-primary' : 'text-slate-500 hover:text-primary'}`}
      >
        <Home size={24} className={currentView === 'home' ? 'fill-current' : ''} />
        <span className="text-[10px] font-bold">Unlock</span>
      </button>
      <button 
        onClick={() => onNavigate('devices')}
        className={`flex flex-col items-center gap-1 p-2 transition-colors ${currentView === 'devices' ? 'text-primary' : 'text-slate-500 hover:text-primary'}`}
      >
        <Smartphone size={24} className={currentView === 'devices' ? 'fill-current' : ''} />
        <span className="text-[10px] font-bold">Devices</span>
      </button>
      <button 
        onClick={() => onNavigate('add-device')}
        className={`flex flex-col items-center gap-1 p-2 transition-colors ${currentView === 'add-device' ? 'text-primary' : 'text-slate-500 hover:text-primary'}`}
      >
        <PlusCircle size={24} className={currentView === 'add-device' ? 'fill-current' : ''} />
        <span className="text-[10px] font-bold">Add</span>
      </button>
    </nav>
  );
}
