import { defineStore } from 'pinia'
import { LessonPlan, LessonPlanType } from '../../common/LessonStates';

export const useLessonStore = defineStore({
    id: 'lesson',
    state: () => ({
        count: 0,
        current_lesson: [] as LessonPlanType,
    }),
    getters: {
        isReady: (state) => {return state.current_lesson.length > 0},
        currentLessonState (state) {
            if (this.isReady == false) return null;
            if (state.count < 0 || state.count >= state.current_lesson.length) return null;
            return state.current_lesson[state.count]
        },
        canAdvance: (state) => { return state.count < (state.current_lesson.length-1) },
        canRewind: (state) => { return state.count > 0 },
    },
    actions: {
        advanceLessonState() {
            if (this.canAdvance) this.count+=1;
        },
        rewindLessonState() {
            if (this.canRewind) this.count-= 1;
        },
        reset() {
            this.current_lesson = 0;
            this.count = 0;
        },
        async attemptFileLoad(): Promise<string> {
            try {
                const lesson_json = (await (window as any).electronAPI.openFile()) as (string|null)
                if (lesson_json == null) return "Did not select a file"
                const raw_json = JSON.parse(lesson_json);
                return this.attemptLessonLoad(raw_json);
            } 
            catch (e) {
                console.error(e);
                return "Failed to load file";
            }
        },
        async attemptLessonLoad(raw_lesson:object): Promise<string> {
            try {
                this.current_lesson = [];
                const parsed_lesson_plan = LessonPlan.safeParse(raw_lesson);
                if (parsed_lesson_plan.success == true) {
                    this.current_lesson = parsed_lesson_plan.data;
                }
                else {
                    return parsed_lesson_plan.error.message
                }
            } 
            catch (e) {
                console.error(e);
                return "Failed to load file";
            }
        }
    }
})