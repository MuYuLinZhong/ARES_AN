package com.example.all.data.local

import androidx.room.Dao
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import com.example.all.data.local.entity.DeviceEntity
import kotlinx.coroutines.flow.Flow

/**
 * 设备数据访问对象 —— 定义 device_cache 表的全部数据库操作
 *
 * 所有读操作返回 Flow，数据变化时自动通知观察者（驱动 UI 实时刷新）。
 * 所有写操作为 suspend 函数，在协程中执行（不阻塞主线程）。
 */
@Dao
interface DeviceDao {

    /** 观察全部设备，按昵称排序 —— 用于设备列表页实时展示 */
    @Query("SELECT * FROM device_cache ORDER BY nickname")
    fun observeAll(): Flow<List<DeviceEntity>>

    /** 根据 deviceId 查询单个设备 —— 用于设备详情页 */
    @Query("SELECT * FROM device_cache WHERE deviceId = :id")
    suspend fun getById(id: String): DeviceEntity?

    /** 批量插入或更新 —— Phase 2 云端拉取后整体写入缓存 */
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun upsertAll(devices: List<DeviceEntity>)

    /** 单条插入或更新 —— Phase 1 本地添加设备时使用 */
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    suspend fun upsert(device: DeviceEntity)

    /** 标记设备为无效 —— Phase 2 权限撤销时调用，UI 端灰化展示 */
    @Query("UPDATE device_cache SET isValid = 0 WHERE deviceId = :id")
    suspend fun markInvalid(id: String)

    /** 删除单个设备 —— 用户主动解绑时调用 */
    @Query("DELETE FROM device_cache WHERE deviceId = :id")
    suspend fun deleteById(id: String)

    /** 清空全表 —— 登出或注销时调用 */
    @Query("DELETE FROM device_cache")
    suspend fun deleteAll()
}
