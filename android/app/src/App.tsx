/**
 * @license
 * SPDX-License-Identifier: Apache-2.0
 */

import { useState } from 'react';
import Login from './views/Login';
import Home from './views/Home';
import Devices from './views/Devices';
import DeviceDetails from './views/DeviceDetails';
import AddDevice from './views/AddDevice';
import Settings from './views/Settings';
import UpdatePassword from './views/UpdatePassword';

export type View = 'login' | 'home' | 'devices' | 'device-details' | 'add-device' | 'settings' | 'update-password';

export default function App() {
  const [currentView, setCurrentView] = useState<View>('login');
  const [selectedDeviceId, setSelectedDeviceId] = useState<string | null>(null);

  const navigate = (view: View, data?: any) => {
    if (data?.deviceId) {
      setSelectedDeviceId(data.deviceId);
    }
    setCurrentView(view);
  };

  return (
    <div className="w-full min-h-screen bg-background-dark text-slate-100 flex justify-center">
      <div className="w-full max-w-md relative min-h-screen bg-background-dark shadow-2xl overflow-hidden flex flex-col">
        {currentView === 'login' && <Login onNavigate={navigate} />}
        {currentView === 'home' && <Home onNavigate={navigate} />}
        {currentView === 'devices' && <Devices onNavigate={navigate} />}
        {currentView === 'device-details' && <DeviceDetails onNavigate={navigate} deviceId={selectedDeviceId} />}
        {currentView === 'add-device' && <AddDevice onNavigate={navigate} />}
        {currentView === 'settings' && <Settings onNavigate={navigate} />}
        {currentView === 'update-password' && <UpdatePassword onNavigate={navigate} />}
      </div>
    </div>
  );
}
