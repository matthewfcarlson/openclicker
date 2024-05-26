import {z} from "zod";

export enum LessonStateNames {
    Introduction = "intro",
    Section = "section"
}
const LessonStateNamesZ = z.nativeEnum(LessonStateNames);

const Base = z.object({
    type: LessonStateNamesZ
})

export const LessonIntro = Base.extend({
        type: z.literal(LessonStateNames.Introduction),
        text: z.string()
    });

export const LessonSection = Base.extend({
        type: z.literal(LessonStateNames.Section),
        text: z.string(),
        photo: z.string().url().optional(),
    });

export const AnyLessonState = z.discriminatedUnion("type", [LessonIntro,LessonSection] )
export type AnyLessonStateType = z.infer<typeof AnyLessonState>;

export const LessonPlan = z.array(AnyLessonState)

export type LessonPlanType = z.infer<typeof LessonPlan>;
